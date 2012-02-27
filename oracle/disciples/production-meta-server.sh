#!/bin/sh
# @file production-meta-server.sh
# @author <raphael.londeix@gmail.com> Raphael Londeix
# @version @FIXME@

SCRIPT=`basename "$0"`
SCRIPTDIR=`python -c "import os;print os.path.abspath(os.path.dirname('$0'))"`

kill_screen_session()
{
	if [ -n "`screen -ls | grep $1`" ]
	then
		echo "Killing $1 screen session"
		screen -S INFINIT_MONGODB_SERVER -X quit
		while [ -n "`screen -ls | grep $1`" ]
		do
			echo "Waiting $1 screen session to be closed"
			sleep 1
		done
	fi
}

start_screen()
{
	screen -S $1 -m -d $2
}

restart_screen()
{
	kill_screen_session $1
	start_screen $1 "$2"
}

restart_screen INFINIT_MONGODB_SERVER "mongod --dbpath /tmp"

kill_screen_session INFINIT_META_SERVER

try=0
max_tries=10
while true
do
	echo "Launching meta server"
	screen -L -S INFINIT_META_SERVER -m -d "$SCRIPTDIR"/meta-server
	before=`screen -ls | grep INFINIT_META_SERVER`
	sleep 1
	after=`screen -ls | grep INFINIT_META_SERVER`
	echo "'$before' '$after'"
	[ -z "$after" ] && echo BIET
	[ -n "`screen -ls | grep INFINIT_META_SERVER`" ] && exit 0
	if [ $try = $max_tries ]
	then
		echo "Stop trying ..."
		exit 1
	fi
	try=`expr $try + 1`
	echo "Retrying to start meta server (try $try)"
	[ -f "$SCRIPTDIR"/screenlog.0 ] && (
		cat "$SCRIPTDIR"/screenlog.0
		rm "$SCRIPTDIR"/screenlog.0
	)
done
