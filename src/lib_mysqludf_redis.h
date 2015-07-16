/**
 * @file lib_mysqudf_redis.h
 * @brief The mysql udf for redis
 * Created by rei
 *
 * libmysq4redis - mysql udf for redis
 * Copyright (C) 2015 Xi'an Tomoon Tech Co.,Ltd. All rights are served.
 * web: http://www.tomoon.cn
 *
 * @author rei  mail: wurei@126.com
 */

#ifdef STANDARD
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong;
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>
#endif
#include <mysql.h>
#include <m_ctype.h>
#include <m_string.h>
#include <stdlib.h>

#include <ctype.h>
#include <stdarg.h>
#include <unistd.h>

#include <pthread.h>

/**
 * hiredis head file
 */
#include "hiredis/hiredis.h"
#include "thread_pool.h"

/**
 * mysql head file
 */
#include <mysql/plugin.h>

//fix me!
#ifdef	__cplusplus
extern "C" {
#endif

#define LIBVERSION "lib_mysqludf_redis version 0.2.0"
#define MAX_LEN 8192 // max allowed length of input string, including 0 terminator
#define MAX_STR 16 // max allowed number of substrings in input string

#ifdef __WIN__
#define SETENV(name,value)		SetEnvironmentVariable(name,value);
#else
#define SETENV(name,value)		setenv(name,value,1);		
#endif

struct redis_command {
    size_t arg_count; /* Number of arguments */
    char **argv; /* Pointer to argument */
    size_t *argvlen; /* lengths of arguments */
};

enum conn_type {
    CONN_TCP, CONN_UNIX_SOCK
};

struct config {
    enum conn_type type;

    struct {
        char host[256];
        int port;
    } tcp;

    struct {
        const char *path;
    } unix_sock;
    char password[256];
    int auth;
    char log_file[256];
    int debug;
    int worker;
};

static struct config cfg = { .tcp = { .host = "127.0.0.1", .port = 6379 },
        .unix_sock = { .path = "/tmp/redis.sock" }, .password = "", .auth = 0,
        .log_file = "/tmp/redis_udf.log", .debug = 0, .type = CONN_TCP,
        .worker = 1 };

typedef struct _mysql4redis_t mysql4redis_t;

/**
 set server info command.
 */
my_bool redis_servers_init(UDF_INIT *initid, UDF_ARGS *args,
        char *message);

void redis_servers_deinit(UDF_INIT *initid);

my_ulonglong redis_servers(UDF_INIT *initid, UDF_ARGS *args,
        char *is_null, char *error);

/**
 * redis_command
 * 
 * execute multiple redis command (ex: select 1\n set x 1\n)
 */
my_bool redis_command_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

void redis_command_deinit(UDF_INIT *initid);

my_ulonglong redis_command(UDF_INIT *initid, UDF_ARGS *args, char *is_null,
        char *error);

/**
 free resources.
 */
my_bool redis_destory_init(UDF_INIT *initid, UDF_ARGS *args, char *message);

void redis_destory_deinit(UDF_INIT *initid);

my_ulonglong redis_destory(UDF_INIT *initid, UDF_ARGS *args, char *is_null,
        char *error);

void free_command(struct redis_command *cmd);

redisContext *redis_context_new();
void redis_context_destory(redisContext *rc);

#define debug_print(...) \
   do { \
	   if (cfg.debug && mysql4redis->log_file) {\
	   	   fprintf(mysql4redis->log_file,  __VA_ARGS__);fflush(mysql4redis->log_file);\
	   } \
   } while (0)

#define info_print(...) \
   do { \
	   if (mysql4redis->log_file) {\
	   	   fprintf(mysql4redis->log_file,  __VA_ARGS__);fflush(mysql4redis->log_file); \
	   } \
   } while (0)

#ifdef	__cplusplus
}
#endif

