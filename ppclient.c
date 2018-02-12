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

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "options.h"
#include "socket.h"
#include "protocol.h"

#define MAX_RECV_LEN 10

int main(int argc, char **argv)
{
    options_t *opts = options(argc, argv);
    opts->n_threads = 1;

    socket_t *so;
    if ((so = socket_init(opts->host, opts->port)) != NULL) {
        if (opts->verbose)
            printf("Opened connection to %s:%s\n", opts->host, opts->port);

        if (socket_connect(so) == 0) {

            size_t rlen = 0;
            const char *req = proto_request(&rlen);
            ssize_t nbytes;

            if ((nbytes = send(so->sd, req, rlen, 0)) >= 0) {
                if (opts->verbose)
                    printf("Sent %s (%ld bytes)\n", req, nbytes);

                char buf[MAX_RECV_LEN];
                memset(buf, 0, MAX_RECV_LEN);

                if ((nbytes = recv(so->sd, buf, MAX_RECV_LEN - 1, 0)) >= 0) {
                    if (opts->verbose)
                        printf("Recv %s (%ld bytes).\n", buf, nbytes);
                }
            }
        }
        if (opts->verbose)
            printf("Closed connection to %s:%s\n", opts->host, opts->port);
        socket_cleanup(so);
    }
    return 0;
}
