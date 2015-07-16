/**
 * @file lib_mysqludf_redis.c
 * @brief The mysql udf for redis
 * Created by rei
 *
 * libmysq4redis - mysql udf for redis
 * Copyright (C) 2015 Xi'an Tomoon Tech Co.,Ltd. All rights are served.
 * web: http://www.tomoon.cn
 *
 * @author rei  mail: wurei@126.com
 */

#include "lib_mysqludf_redis.h"

struct _mysql4redis_t {
    threadpool *thread_pool;
    FILE *log_file;
    void *user_data;
};

//static pthread_mutex_t redis_context_mutex = PTHREAD_MUTEX_INITIALIZER;

static mysql4redis_t *mysql4redis = NULL;

void *
_do_redis_command(thread *thread_p, void *data)
{
    redisContext *rc = thread_p->rc;
    redisReply *reply = NULL;
    struct redis_command *command = (struct redis_command *) data;

    info_print("run redis command begin\n");
    if (!rc || !command) {
        info_print("_redis_context_init return null, connect failed\n");
        goto failed;
    }

    reply = redisCommandArgv(rc, command->arg_count,
            (const char **) command->argv, (const size_t *) command->argvlen);
    info_print("run redis command normal\n");
    if (!reply) {
        info_print("_do_redis_command failed\n");
        goto failed;
    } else {
        freeReplyObject(reply);
    }
    info_print("run redis command end\n");

failed:
    free_command(command);
    command = NULL;

    return NULL;
}

void free_command(struct redis_command *cmd)
{
    int i = 0;

    if (cmd) {
        debug_print("free command\n");
        for (i = 0; i < cmd->arg_count; i++) {
            if (cmd->argv[i])
                free(cmd->argv[i]);
        }
        if (cmd->argv)
            free(cmd->argv);
        if (cmd->argvlen)
            free(cmd->argvlen);
        free(cmd);
    }
}

void mysql4redis_destory()
{
    if (mysql4redis) {
        if (mysql4redis->thread_pool) {
            thpool_destroy(mysql4redis->thread_pool);
            mysql4redis->thread_pool = NULL;
        }
        if (mysql4redis->log_file) {
            fclose(mysql4redis->log_file);
            mysql4redis->log_file = NULL;
        }
        free(mysql4redis);
        mysql2redis = NULL;
    }
}

mysql4redis_t *
mysql4redis_new()
{
    if (mysql4redis) {
        return mysql2redis;
    }
    mysql4redis = (mysql4redis_t *)malloc(sizeof(mysql4redis_t));
    if (mysql4redis == NULL) {
        printf("init mysql2redis failed!\n");
        return NULL;
    }
    mysql4redis->log_file = NULL;
    if (cfg.debug) {
        mysql4redis->log_file = fopen(cfg.log_file, "a+");
    }
    mysql4redis->thread_pool = (threadpool *) thpool_init(cfg.worker);
    if (mysql4redis->thread_pool == NULL) {
        mysql4redis_destory(mysql4redis);
        return NULL;
    }
    return mysql4redis;
}

redisContext *
redis_context_new()
{
    redisContext *rc = NULL;
    redisReply *reply;
    char cmd[256] = { 0 };
    int len = 0;

    info_print("Connection redis\n");
    if (cfg.type == CONN_TCP) {
        rc = redisConnect(cfg.tcp.host, cfg.tcp.port);
    } else if (cfg.type == CONN_UNIX_SOCK) {
        rc = redisConnectUnix(cfg.unix_sock.path);
    }

    if (rc->err && mysql4redis->log_file) {
        info_print("Connection error: %s\n", rc->errstr);
        return rc;
    }

    if (cfg.auth) {
        info_print("Connection redis\n");
        reply = redisCommand(rc, "AUTH %s", cfg.password);
        if (reply) {
            freeReplyObject(reply);
            reply = NULL;
        }
    }

    if (rc)
        info_print("Connection success\n");

    return rc;
}

void redis_context_destory(redisContext *rc)
{
    if (rc) {
        redisFree(rc);
        rc = NULL;
    }
}

my_bool redis_servers_init(UDF_INIT *initid __attribute__((__unused__)),
        UDF_ARGS *args, char *message)
{
    //pthread_mutex_lock(&redis_context_mutex);

    if (args->arg_count < 2 || args->arg_type[0] != STRING_RESULT
            || args->arg_type[1] != INT_RESULT) {
        strncpy(message, "Usage:host port", MYSQL_ERRMSG_SIZE);
        goto failed;
    }

    if (args->arg_count == 3 && args->arg_type[2] != STRING_RESULT) {
        strncpy(message, "Usage:host port password", MYSQL_ERRMSG_SIZE);
        goto failed;
    }

    strncpy(cfg.tcp.host, (char *) args->args[0], sizeof(cfg.tcp.host));
    cfg.tcp.port = *((longlong *) args->args[1]);
    if (args->arg_count == 3) {
        cfg.auth = 1;
        strncpy(cfg.password, (char *) args->args[2], sizeof(cfg.password));
    }

    if (mysql4redis_new() == NULL) {
        strncpy(message, "init failed", MYSQL_ERRMSG_SIZE);
        goto failed;
    }

    debug_print("init redis success\n");

    //pthread_mutex_unlock(&redis_context_mutex);
    return 0;

failed:
    //pthread_mutex_unlock(&redis_context_mutex);
    return -1;
}

void redis_servers_deinit(UDF_INIT *initid __attribute__((unused)))
{
    debug_print("redis deinit");
}

my_ulonglong redis_servers(UDF_INIT *initid __attribute__((__unused__)),
        UDF_ARGS *args, char *is_null __attribute__((__unused__)),
        char *error __attribute__((__unused__)))
{
    return 0;
}

my_bool redis_command_init(UDF_INIT *initid __attribute__((__unused__)),
        UDF_ARGS *args, char *message)
{
    if (args->arg_count >= 3 && args->arg_type[0] == STRING_RESULT
            && args->arg_type[1] == STRING_RESULT
            && args->arg_type[2] == STRING_RESULT) {
        args->maybe_null = NULL;
        if (!mysql4redis) {
            snprintf(message, MYSQL_ERRMSG_SIZE, "redis not init");
            return 1;
        }
        return 0;
    } else {
        snprintf(message, MYSQL_ERRMSG_SIZE,
                "redis_command(cmd,arg1,arg2,[...])");
    }
    return 1;
}

void redis_command_deinit(UDF_INIT *initid __attribute__((__unused__)))
{
    debug_print("redis_command_deinit\n");
}

my_ulonglong redis_command(UDF_INIT *initid __attribute__((__unused__)),
        UDF_ARGS *args, char *is_null __attribute__((__unused__)),
        char *error __attribute__((__unused__)))
{
    int i = 0, res = 0, r = 0;
    struct redis_command *command = malloc(sizeof(struct redis_command));
    if (command == NULL) {
        return r;
    }
    command->argv = malloc(args->arg_count * sizeof(void *));
    if (command->argv == NULL) {
        free(command);
        return r;
    }
    command->argvlen = malloc(args->arg_count * sizeof(size_t));
    if (command->argvlen == NULL) {
        free(command->argv);
        return r;
    }
    command->arg_count = args->arg_count;

    debug_print("number of args %d\n", args->arg_count);

    for (i = 0; i < args->arg_count; i++) {
        command->argv[i] = malloc(args->lengths[i]);
        if (command->argv[i] == NULL) {
            free_command(command);
        }
        memcpy(command->argv[i], args->args[i], args->lengths[i]);
        command->argvlen[i] = args->lengths[i];
    }

    if (mysql4redis->thread_pool) {
        res = thpool_add_work(mysql4redis->thread_pool, _do_redis_command,
                command);
        if (res == -1) {
            free_command(command);
            command = NULL;
        }
        info_print("queue size=%d\n", thpool_get_queue_size(mysql4redis->thread_pool));
    }

    return r;
}

my_bool redis_destory_init(UDF_INIT *initid __attribute__((__unused__)),
        UDF_ARGS *args, char *message)
{
    return 0;
}

void redis_destory_deinit(UDF_INIT *initid __attribute__((unused)))
{
    debug_print("redis_command_deinit\n");
}

my_ulonglong redis_destory(UDF_INIT *initid __attribute__((__unused__)),
        UDF_ARGS *args, char *is_null __attribute__((__unused__)),
        char *error __attribute__((__unused__)))
{
    mysql4redis_destory();
    return 0;
}
