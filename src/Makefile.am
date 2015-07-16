lib_LTLIBRARIES = libmysql2redis.la

libmysql2redis_la_SOURCES = \
	lib_mysqludf_redis.c \
	thread_pool.c \
    hiredis/async.c \
    hiredis/dict.c \
    hiredis/hiredis.c \
    hiredis/net.c \
    hiredis/read.c \
    hiredis/sds.c

libmysql2redis_la_CFLAGS = `mysql_config --cflags`
libmysql2redis_la_LDFLAGS = -version-info 1:0:0 -no-undefined `mysql_config --libs`

libmysql2redisincludedir = ${includedir}
libmysql2redisinclude_HEADERS = lib_mysqludf_redis.h
