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

#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <omp.h>

#include "socket.h"
#include "server.h"
#include "response.h"

int server_create(const char *host, const char *port, int backlog)
{
    socket_t *srv;

    int retval = 0;
    if ((srv = socket_init(NULL, port)) == NULL) {
        perror("Cannot create socket.");
        retval = EINVAL;
        goto cleanup;
    }

    if (socket_listen(srv, (unsigned int) backlog) != 0) {
        perror("Cannot create socket");
        retval = EINVAL;
        goto cleanup;
    }

    printf("Server running at %s:%s\n", host, port);

    while (true) {
        omp_set_num_threads(backlog);
        #pragma omp parallel
        {
            int pfd = socket_accept(srv);
            {
                if (pfd < 0) {
                    fprintf(stderr, "Cannot accept.");
                } else {
                    server_respond(pfd);
                }
            }
        }
    };

    cleanup:
    if (srv != NULL)
        socket_cleanup(srv);

    return retval;
}
