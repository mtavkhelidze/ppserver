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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>

#ifdef __linux__
    #include <unistd.h>
#endif

#include "config.h"
#include "options.h"

options_t opts = {
    .n_threads = 1,
    .backlog = PP_BACKLOG,
    .ttl = PP_TTL,
    .host = PP_HOST,
    .port = PP_PORT
};

void usage(int code)
{
    FILE *ff = code == 0 ? stdout : stderr;
    if (code == 0)
        fprintf(ff, "%s by %s\n\n", PACKAGE_STRING, PACKAGE_AUTHOR);

#ifdef PP_SERVER
    const char *header = "usage:\t%s [-H HOST] [-p PORT] "
        "[-t THREADS] [-T TTL] [-b BACKLOG] [-v] [-h]\n\n";
#else
    const char *header = "usage:\t%s [-H HOST] [-p PORT] [-t THREADS] "
                         "[-v] [-h]\n\n";
#endif
    fprintf(ff, header, PACKAGE_NAME);

    if (code == 0) {
        fprintf(
            ff, "Options:\n"
                "\t-H\thostname or IPv[4|6] address (default %s)\n"
                "\t-p\tport (default %s)\n"
                "\t-t\tnumber of threads (default: number of"
                " online CPUs, %d on this system)\n"
                #ifdef PP_SERVER
                "\t-T\tinactive connection TTL in seconds (default %d)\n"
                    "\t-b\taccept backlog (default %d)\n"
                #endif
                "\t-v\tbe a little bit verbose\n"
                "\t-h\tthis help message\n\n"
#ifdef PP_SERVER
        "\tNB:\tUse different combinations of -t and -b\n"
        "\t\tto tune this server's performance on your system.\n"
#endif
            , opts.host, opts.port, opts.n_threads
#ifdef PP_SERVER
            , opts.ttl, opts.backlog
#endif
        );
    }
    exit(code);
}

options_t *options(int argc, char **argv)
{

    int c;

    opts.n_threads = (int) sysconf(_SC_NPROCESSORS_ONLN);

#ifdef PP_SERVER
    const char *ostr = "+vH:p:T:b:t:h?";
#else
    const char *ostr = "+vH:p:t:h?";
#endif

    while ((c = getopt(argc, argv, ostr)) != -1) {
        switch (c) {
            case 'h':
                usage(EXIT_SUCCESS);
            case '?':
                fprintf(stderr, "Invalid option -%c\n", optopt);
                usage(EINVAL);
            case 'v':
                opts.verbose = true;
                break;
            case 'H':
                opts.host = optarg;
                break;
            case 'p':
                if ((int) strtol(optarg, (char **) NULL, 10) <= 0) {
                    fprintf(stderr, "Invalid port number\n");
                    usage(EINVAL);
                }
                opts.port = optarg;
                break;
#ifdef PP_SERVER
            case 'T': {
                int t = (int) strtol(optarg, (char **) NULL, 10);
                if (t > 0)
                    opts.ttl = t;
            }
                break;
            case 'b': {
                int bl = (int) strtol(optarg, (char **) NULL, 10);
                if (bl > 0)
                    opts.backlog = bl;
            }
#endif
                break;
            case 't': {
                int nt = (int) strtol(optarg, (char **) NULL, 10);
                if (nt > 0)
                    opts.n_threads = nt;
            }
                break;
            default:
                /*
                 * this should never happen :)
                 */
                abort();
        }
    }
    return &opts;
}
