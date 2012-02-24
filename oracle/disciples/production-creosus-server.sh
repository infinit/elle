#!/bin/sh
DIR="`dirname \"$0\"`"
PIDFILE=/var/run/infinit-creosus.pid
[ ! -e $PIDFILE ] && sudo touch $PIDFILE && sudo chown www-data $PIDFILE
PID="`cat $PIDFILE`"
[ -n "$PID" ] && echo "Killing $PID" && sudo kill "$PID"
sleep 1
export ENABLE_FASTCGI=1
sudo -u www-data spawn-fcgi -a 127.0.0.1 -p 7880 -P $PIDFILE -F 1 "$DIR"/creosus-server 127.0.0.1:8089
