/**
 * Copyright (c) 2018-2019 Misha Tavkhelidze <misha.tavkhelidze@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>

#ifdef __linux__
    #include <signal.h>
#endif

#include "config.h"
#include "socket.h"
#include "response.h"
#include "sig_handler.h"
#include "options.h"
#include "protocol.h"
#include "util.h"

extern options_t opts;

int _response_hup(int pfd)
{
    size_t rlen = 0;
    const char *res = proto_hup(&rlen);
    if (send(pfd, res, rlen, 0) < 0) {
        perror("Cannot send");
        return -1;
    }
    return 0;
}

int _response_norm(int pfd, const char *req, ssize_t rlen)
{
    const char *res = proto_response(req, (size_t *) &rlen);
    if (res != NULL) {
        if (send(pfd, res, (size_t) rlen, 0) < 0) {
            perror("Cannot send");
            return -1;
        }
    }
    return 0;
}

void response_talk(void *args)
{
    response_t *r = (response_t *) args;
    int pfd = r->pfd;

    size_t blen = strlen(PP_CLIENT_REQ);
    char buf[blen + 1];
    memset(buf, 0, blen + 1);

    if (opts.verbose)
        report_peer_connection(pfd, true);

    /*
     * Don't terminate on SIGUSR1, just wake
     * up further down from blocking recv(2)
     */
    ignore_signal(SIGUSR1);

    if (set_min_recv_len(pfd, blen) < 0 ||
        set_connection_timeout(pfd, opts.ttl) < 0) {
        goto error;
    };

    do {
        ssize_t nbytes = recv(pfd, buf, blen, 0);

        /*
         * Client closed connection
         */
        if (nbytes == 0) {
            break;
        } else if (nbytes < 0) {
            if (errno == EWOULDBLOCK || errno == EINTR) {
                /*
                 * Either timeout or shutdown happened.
                 */
                _response_hup(pfd);
            } else {
                /*
                 * Error.
                 */
                perror("Cannot receive");
            }
            break;
        }

        if (_response_norm(pfd, buf, nbytes) < 0)
            break;
    } while (true);

error:
    if (opts.verbose)
        report_peer_connection(pfd, false);

    close(pfd);
}
