#!/bin/sh
set -ex
gcc -o libpreload-mysqldump.so -shared libpreload-mysqldump.c -fPIC -ldl
LD_PRELOAD="`dirname $0`/libpreload-mysqldump.so" exec "$@"
