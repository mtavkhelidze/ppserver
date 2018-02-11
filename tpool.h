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

#ifndef PING_PONG_TPOOL_H
#define PING_PONG_TPOOL_H

#include <pthread.h>

typedef enum {
    TPF_NONE = 0x0,
    TPF_SHUTDOWN = 0x1,
    TPF_IGNORE_SIGINT = 0x2
} tpool_flags_t;

typedef void (*worker_routine)(void *);

typedef void *worker_routine_args;

typedef struct _thread_job_t {
    worker_routine worker;
    worker_routine_args args;
    struct _thread_job_t *next;
} tpool_job_t;

typedef struct {
    tpool_flags_t flags;

    int n_threads;
    int n_jobs;

    pthread_t *threads;
    tpool_job_t *j_head;
    tpool_job_t *j_tail;

    pthread_cond_t job_notify;
    pthread_mutex_t pool_lock;
} tpool_t;

tpool_t *tpool_init(int n_threads);
int tpool_destroy(tpool_t *tp);
int tpool_add_job(tpool_t *tp, worker_routine worker, worker_routine_args args);

#endif // PING_PONG_TPOOL_H
