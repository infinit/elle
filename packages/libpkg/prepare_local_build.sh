#!/bin/sh
# @file prepare_local_build.sh
# @author <raphael.londeix@gmail.com> Raphael Londeix
# @version @FIXME@

SCRIPT=`basename "$0"`
SCRIPTDIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`

usage()
{
	echo "Usage: $SCRIPT build_dir install_dir"
	exit 1
}

[ $# = 2 ] || usage

export DIR_SOURCE="$SCRIPTDIR/../.."
export DIR_BUILD="$1"
export DIR_INSTALL="$2"

"$DIR_SOURCE"/farm/install
