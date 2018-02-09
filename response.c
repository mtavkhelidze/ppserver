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
#include <omp.h>
#include <stdbool.h>
#include <unistd.h>

#include "config.h"
#include "socket.h"
#include "response.h"

void server_respond(int pfd)
{
    ssize_t nbytes = 0;
    size_t blen = strlen(PP_CLIENT_REQ);
    char buf[blen + 1];

    memset(buf, 0, blen + 1);

    peer_addr_t *p = peer_addr(pfd);
    do {
        // recv don't return till all bytes are received
        if (setsockopt(pfd, SOL_SOCKET, SO_RCVLOWAT,
                       (char *) &blen, sizeof(blen)) < 0) {
            fprintf(stderr, "setsockopt(SO_RCVLOWAT) failed: %d\n", pfd);
            perror("");
            break;
        }

        if ((nbytes = recv(pfd, buf, blen, 0)) < -1) {
            perror("Cannot receive");
            break;
        }

        printf("Received: |%s| (%ld bytes of %ld bytes)", buf, nbytes, blen);
        if (p != NULL) {
            printf(" from %s on %d\n", p->host, p->port);
            free(p);
        } else {
            printf("\n");
        }

        if (nbytes == blen) {
            if (send(pfd, PP_SERVER_RES, strlen(PP_SERVER_RES), 0) == -1) {
                perror("Cannot send");
                break;
            }
        }
    } while (false);

    close(pfd);
}
