# mysql2redis
The mysql udf for redis.

I found this [mysql2redis](https://github.com/dawnbreaks/mysql2redis), but dependencies [apr](http://apr.apache.org/download.cgi), so rewrite it ^^.

hiredis:https://github.com/redis/hiredis

./autogen.sh

./configure --prefix=/opt/

make && make install
