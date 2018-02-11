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
#include <stdio.h>
#include <errno.h>
#include <sys/param.h>

#include "tpool.h"

#define is_shutdown(x) ((x)->flags & TPF_SHUTDOWN)
#define set_shutdown(x) ((x)->flags |= TPF_SHUTDOWN)

static tpool_job_t *_job_init(functor_t worker, void *args)
{
    tpool_job_t *job;
    if ((job = malloc(sizeof(tpool_job_t))) == NULL) {
        perror("Cannot allocate memory for job");
        abort();
    }

    job->worker = (functor_t )worker;
    job->args = args;
    job->next = NULL;
    return job;
}

static void _add_job(tpool_t *tp, tpool_job_t *job)
{
    if (tp->j_head == NULL) {
        tp->j_head = job;
    } else {
        tp->j_tail->next = job;
    }
    tp->j_tail = job;
    tp->n_jobs++;
}

static void _remove_job(tpool_t *pt)
{
    tpool_job_t *job = pt->j_head;
    pt->j_head = pt->j_head->next;
    pt->n_jobs--;
    free(job);
}

static void _tpool_free(tpool_t *tp)
{
    for (tpool_job_t *j = tp->j_head; j != NULL; j = j->next) {
        free(j->args);
        free(j);
    }
    tp->j_head = NULL;
    tp->j_tail = NULL;
    tp->n_jobs = 0;

    free(tp->threads);
    pthread_mutex_destroy(&(tp->pool_lock));
    pthread_cond_destroy(&(tp->job_notify));

    free(tp);
}

static void *_tpool_thread(void *data)
{
    tpool_t *tp = (tpool_t *) data;

    for (;;) {
        pthread_mutex_lock(&(tp->pool_lock));

        while (tp->n_jobs == 0 && !is_shutdown(tp)) {
            pthread_cond_wait(&(tp->job_notify), &(tp->pool_lock));
        }

        if (is_shutdown(tp)) {
            pthread_mutex_unlock(&(tp->pool_lock));
            return NULL;
        }

        functor_t fn = (functor_t )tp->j_head->worker;
        void *args = tp->j_head->args;
        _remove_job(tp);

        pthread_mutex_unlock(&(tp->pool_lock));

        (*fn)(args);

        free(args);
    }

    return NULL;
}

tpool_t *tpool_init(int n_threads)
{
    tpool_t *tp;
    if ((tp = calloc(sizeof(tpool_t), 1)) == NULL) {
        perror("Cannot allocate pool memory");
        abort();
    }

    tp->flags = TPF_NONE;

    tp->n_jobs = 0;
    tp->j_head = NULL;
    tp->j_tail = NULL;

    if ((tp->threads = malloc(sizeof(pthread_t) * n_threads)) == NULL) {
        perror("Cannot allocate threads in pool");
        abort();
    }

    /*
     * On Linux those two below never return an error, but if they
     * somewhere do, at least, postmortem, we'll have an idea, why.
     */
    if (pthread_cond_init(&(tp->job_notify), NULL) != 0) {
        perror("Cannot init conditional var");
        abort();
    }

    if (pthread_mutex_init(&(tp->pool_lock), NULL) != 0) {
        perror("Cannot init mutex");
        abort();
    }

    if (pthread_mutex_lock(&(tp->pool_lock)) != 0) {
        _tpool_free(tp);
        return NULL;
    }

    tp->n_threads = 0;
    for (int i = 0; i < n_threads; i++) {
        if (pthread_create(&(tp->threads[i]), NULL, _tpool_thread,
                           (void *) tp) != 0) {
            _tpool_free(tp);
            return NULL;
        };
        tp->n_threads++;
    }

    if (pthread_mutex_unlock(&(tp->pool_lock)) != 0) {
        _tpool_free(tp);
        return NULL;
    }

    return tp;
}

int tpool_destroy(tpool_t *tp)
{
    if (tp == NULL) {
        return EINVAL;
    }

    int err = 0;
    if ((err = pthread_mutex_lock(&(tp->pool_lock))) != 0) {
        return err;
    }

    if (is_shutdown(tp)) {
        goto cleanup;
    }
    set_shutdown(tp);

    if ((err = pthread_cond_broadcast(&(tp->job_notify))) != 0) {
        goto cleanup;
    }

    if ((err = pthread_mutex_unlock(&(tp->pool_lock))) != 0) {
        goto cleanup;
    }

    for (int i = 0; i < tp->n_threads; i++) {
        pthread_kill(tp->threads[i], SIGUSR1);
    }

    for (int i = 0; i < tp->n_threads; i++) {
        pthread_join(tp->threads[i], NULL);
    }

cleanup:

    if (err) {
        abort();
    }

    _tpool_free(tp);
    return err;
}

int tpool_add_job(tpool_t *tp, functor_t worker, void *args)
{
    if (tp == NULL || worker == NULL) {
        return EINVAL;
    }

    int err = 0;

    if ((err = pthread_mutex_lock(&(tp->pool_lock))) != 0) {
        return err;
    }

    if (is_shutdown(tp)) {
        err = EBUSY;
        goto cleanup;
    }

    tpool_job_t *job = _job_init(worker, args);
    _add_job(tp, job);

    if ((err = pthread_cond_signal(&(tp->job_notify))) != 0) {
        goto cleanup;
    }

cleanup:
    {
        int u_err;
        if ((u_err = pthread_mutex_unlock(&tp->pool_lock)) != 0) {
            err = u_err;
        };
    }
    return err;
}
