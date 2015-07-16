# mysql2redis
The mysql plugin for redis.

I found this https://github.com/dawnbreaks/mysql2redis, but dependencies apr<href="">, so rewrite it ^^.

hiredis:https://github.com/redis/hiredis

./autogen.sh

./configure --prefix=/opt/

make && make install
