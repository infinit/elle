#!/bin/sh
# @file concurrent_write.2.sh
# @author <raphael.londeix@gmail.com> Raphael Londeix
# @version @FIXME@

SCRIPTCMD="$0"
SCRIPT=`basename "$0"`
SCRIPT_DIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`

abspath()
{
	echo `python3 -c "import os;print(os.path.abspath('$1'))"`
}

fatal()
{
	echo "$*" 2>&1
	exit 1
}

usage()
{
	fatal "Usage: $SCRIPTCMD"
}

fatal This script always fail
