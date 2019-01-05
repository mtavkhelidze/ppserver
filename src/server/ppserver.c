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
#include <signal.h>

#include "config.h"
#include "server.h"
#include "sig_handler.h"

int main(int argc, char **argv)
{
    options_t *opts = options(argc, argv);

    sigset_t sigset;
    sigset_t oldset;
    sigemptyset(&sigset);
    sigemptyset(&oldset);

    /*
     * Block SIGINT catching for subsequent threads
     */
    sigaddset(&sigset, SIGINT);
    pthread_sigmask(SIG_BLOCK, &sigset, &oldset);

    tpool_t *tp = tpool_init(opts->n_threads);
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
    printf(">>>>> %s %s\n", opts->host, opts->port);
    int ret = server_create(tp, opts);

    tpool_destroy(tp);
    printf("\n%s\n", PP_SERVER_HUP);

    return ret;
}
