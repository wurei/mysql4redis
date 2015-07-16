/**
 * @file thread_pool.h
 * @brief The thread pool for mysql to redis
 * Created by rei
 *
 * libmysq2redis - mysql udf for redis
 * Copyright (C) 2015 Xi'an Tomoon Tech Co.,Ltd. All rights are served.
 * web: http://www.tomoon.cn
 *
 * @author rei  mail: wurei@126.com
 */

#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#include "hiredis/hiredis.h"

typedef struct thread {
    int id;
    pthread_t pthread;
    struct thpool_* thpool_p;
    redisContext *rc;
} thread;

typedef struct thpool_ threadpool;

threadpool *thpool_init(int num_threads);

int thpool_add_work(threadpool *,
        void *(*function_p)(struct thread *thread_p, void*), void* arg_p);
void thpool_wait(threadpool *);
void thpool_pause(threadpool *);
void thpool_resume(threadpool *);
void thpool_destroy(threadpool *);
int thpool_get_queue_size(threadpool *);


#endif
