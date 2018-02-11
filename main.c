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
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "config.h"
#include "server.h"
#include "signal.h"

int N_CPUS = 1;

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
                        "\t-H\thostname or IPv[4|6] address (default %s)\n"
                        "\t-p\tport (default %s)\n"
                        "\t-T\tinactive connection TTL in seconds (default %s)\n"
                        "\t-t\tnumber of threads in pool (default: number of"
                        " online CPUs, %d on this system)\n"
                        "\t-b\taccept backlog (default %d)\n"
                        "\t-h\tthis help message\n\n"
                        "\tNB:\tUse different combinations of -t and -b\n"
                        "\t\tto tune this server's performance on your system.\n",
                PP_HOST, PP_PORT, PP_TTL, N_CPUS, PP_BACKLOG);
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
    int ttl = PP_TTL;
    N_CPUS = (int) sysconf(_SC_NPROCESSORS_ONLN);

    int c;
    while ((c = getopt(argc, argv, "+sH:p:T:b:t:h?")) != -1) {
        switch (c) {
            case 'h':
                usage(EXIT_SUCCESS);
            case '?':
                fprintf(stderr, "Invalid option -%c\n", optopt);
                usage(EINVAL);
            case 's':
                server_mode = true;
                break;
            case 'H':
                host = optarg;
                break;
            case 'p':
                port = optarg;
                break;
            case 'T': {
                int t = (int) strtol(optarg, (char **) NULL, 10);
                if (t > 0)
                    ttl = t;
            }
                break;
            case 'b': {
                int bl = (int) strtol(optarg, (char **) NULL, 10);
                if (bl > 0)
                    backlog = bl;
            }
                break;
            case 't': {
                int nt = (int) strtol(optarg, (char **) NULL, 10);
                if (nt > 0)
                    N_CPUS = nt;
            }
                break;
            default:
                /*
                 * this should never happen :)
                 */
                abort();
        }
    }

    /*
     * Block SIGINT catching for subsequent threads
     */
    sigset_t sigset = { 0 };
    sigset_t oldset = { 0 };
    sigaddset(&sigset, SIGINT);
    pthread_sigmask(SIG_BLOCK, &sigset, &oldset);

    tpool_t *tp = tpool_init(N_CPUS);
    printf("Created thread pool with %d workers.\n", tp->n_threads);

    /*
     * Ignore those signals for the current thread.
     */
    ignore_signal(SIGINT);
    ignore_signal(SIGUSR1);

    /*
     * Restore normal sigmast for this thread, but block SIGUSR1
     */
    pthread_sigmask(SIG_SETMASK, &oldset, NULL);

    /*
     * This could probably have it's own separate thread
     */
    int ret = server_mode
              ? server_create(host, port, backlog, ttl, tp)
              : client(host, port);

    tpool_destroy(tp);
    printf("\n%s\n", PP_SERVER_HUP);

    return ret;
}
