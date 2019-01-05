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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "socket.h"
#include "response.h"
#include "tpool.h"
#include "options.h"

int server_create(tpool_t *tp, options_t *opts)
{
    socket_t *srv;

    int retval = 0;
    if ((srv = socket_init(opts->host, opts->port)) == NULL) {
        perror("Cannot create socket");
        retval = EINVAL;
        goto cleanup;
    }

    if (socket_listen(srv, opts->backlog) != 0) {
        perror("Cannot create socket");
        retval = EINVAL;
        goto cleanup;
    }

    printf(
        "Accepting connections on %s:%s (backlog: %d, ttl: %d).\n",
        opts->host, opts->port, opts->backlog, opts->ttl
    );

    for (;;) {
        int pfd;

        if ((pfd = socket_accept(srv)) < 0) {
            if (errno == EINTR) {
                retval = 0;
                goto cleanup;
            }
            perror("Cannot accept");
        } else {
            response_t *r;

            if ((r = malloc(sizeof(response_t))) == NULL) {
                perror("Cannot allocate response_t");
                return EINVAL;
            }

            r->pfd = pfd;
            tpool_add_job(tp, response_talk, (void *) r);
        }
    };

cleanup:
    if (srv != NULL)
        socket_cleanup(srv);

    return retval;
}
