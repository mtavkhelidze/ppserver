/**
 * Copyright (c) 2018 Misha Tavkhelidze <misha.tavkhelidze@gmail.com>
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
#include <pthread.h>
#include <unistd.h>

#ifdef __linux__
    #include <signal.h>
#endif

#include "config.h"
#include "socket.h"
#include "response.h"
#include "signal.h"

static int _set_minimum_recv_size(int pfd, size_t blen)
{
    int ret = 0;
    if ((ret = setsockopt(pfd, SOL_SOCKET, SO_RCVLOWAT, (const void *) &blen,
                          sizeof(blen))) < 0) {
        perror("setsockopt(SO_RCVLOWAT) failed");
    }
    return ret;
}

static int _set_connection_timeout(int pfd, int tout)
{
    int ret = 0;
    struct timeval tv = { 0 };
    tv.tv_sec = tout;
    if ((ret = setsockopt(pfd, SOL_SOCKET, SO_RCVTIMEO, (const void *) &tv,
                          sizeof(tv))) < 0) {
        perror("setsockopt(SO_RCVTIMEO) failed");
    }
    return ret;
}

static void _report_connection(int pfd)
{
    peer_addr_t *p;
    p = peer_addr(pfd);
    printf("%p: Connection %d: from %s on port %d\n", pthread_self(), pfd,
           p->host, p->port);
    free(p);
}

int _response_hup(int pfd)
{
    if (send(pfd, PP_SERVER_HUP, strlen(PP_SERVER_HUP), 0) < 0) {
        perror("Cannot send");
        return -1;
    }
    return 0;
}

int _response_norm(int pfd)
{
    if (send(pfd, PP_SERVER_RES, strlen(PP_SERVER_RES), 0) < 0) {
        perror("Cannot send");
        return -1;
    }
    return 0;
}

void response_talk(void *args)
{
    response_t *r = (response_t *) args;
    int pfd = r->pfd;
    free(args);

    size_t blen = strlen(PP_CLIENT_REQ);
    char buf[blen + 1];
    memset(buf, 0, blen + 1);

    //_report_connection(pfd);

    /*
     * Don't terminate on SIGUSR1, just wake
     * up further down from blocking recv(2)
     */
    ignore_signal(SIGUSR1);

    do {
        if (_set_minimum_recv_size(pfd, blen) < 0 ||
            _set_connection_timeout(pfd, 60) < 0) {
            break;
        };

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

        if (_response_norm(pfd) < 0)
            break;
    } while (false);

    close(pfd);
}
