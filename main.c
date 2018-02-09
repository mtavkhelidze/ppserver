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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <errno.h>
#include "config.h"
#include "server.h"

void usage(int code)
{
    FILE *ff = code == 0 ? stdout : stderr;
    if (code == 0)
        fprintf(ff, "%s by %s\n\n", PACKAGE_STRING, PACKAGE_AUTHOR);

    fprintf(ff, "usage:\t%s [-s] [-H HOST] [-p PORT] [-b BACKLOG] [-h]\n\n",
            PACKAGE_NAME);

    if (code == 0) {
        fprintf(ff, "Options:\n"
                        "\t-s\tserver mode (default false, client mode)\n"
                        "\t-H\thost (default %s)\n"
                        "\t-p\tport (default %s)\n"
                        "\t-b\taccept backlog (default %d)\n"
                        "\t-h\tthis help message\n",
                PP_HOST, PP_PORT, PP_BACKLOG);
    }
    exit(code);
}

int client(const char *host, const char *port)
{
    printf("Client to %s:%s\n", host, port);
    return 0;
}

int main(int argc, char **argv)
{
    bool server_mode = false;
    char *restrict host = PP_HOST;
    char *restrict port = PP_PORT;
    int backlog = PP_BACKLOG;

    int c;
    while ((c = getopt(argc, argv, "+sH:p:b:h?")) != -1) {
        switch (c) {
            case 's':
                server_mode = true;
                break;
            case 'H':
                host = optarg;
                break;
            case 'p':
                port = optarg;
                break;
            case 'b': {
                int bl = (int) strtol(optarg, (char **) NULL, 10);
                if (bl > 0)
                    backlog = bl;
            }
                break;
            case 'h':
                usage(0);
            case '?':
                fprintf(stderr, "Invalid option -%c\n", optopt);
                usage(EINVAL);
            default:
                /* this should never happen :) */
                abort();
        }
    }

    return server_mode
           ? server_create(host, port, backlog)
           : client(host, port);
}
