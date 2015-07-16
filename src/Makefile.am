lib_LTLIBRARIES = libmysql4redis.la

libmysql4redis_la_SOURCES = \
	lib_mysqludf_redis.c \
	thread_pool.c \
    hiredis/async.c \
    hiredis/dict.c \
    hiredis/hiredis.c \
    hiredis/net.c \
    hiredis/read.c \
    hiredis/sds.c

libmysql4redis_la_CFLAGS = `mysql_config --cflags`
libmysql4redis_la_LDFLAGS = -version-info 1:0:0 -no-undefined `mysql_config --libs`

libmysql4redisincludedir = ${includedir}
libmysql4redisinclude_HEADERS = lib_mysqludf_redis.h
