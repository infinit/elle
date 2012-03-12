#!/bin/sh
# @file local_deploy.sh
# @author <raphael.londeix@gmail.com> Raphael Londeix
# @version @FIXME@

SCRIPT=`basename "$0"`
SCRIPTDIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`

export DIR_SOURCE="$SCRIPTDIR"
for build_dir in "$DIR_SOURCE/build"/*
do
	platform=`basename "$build_dir"`
	tmp=`mktemp -d`
	echo "=== Prepare $platform release"
	res=`(
		export DIR_BUILD="$build_dir"
		export DIR_INSTALL="$tmp"
		"$DIR_SOURCE"/farm/install
	) 2>&1`
	if [ $? != 0 ]
	then
		echo '$res'
		exit 1
	fi
	echo "=== $platform release installed in $tmp"
	(
		cd "$tmp/client"
		echo "=== Creating release.tbz"
		tar cjf release.tgz *
		size=`du -hs release.tgz | cut -f1`
		echo "=== Uploading tarball ($size)"
		scp release.tgz oracle@infinit.im:www/infinit.im/downloads/$platform
		echo "=== Deploying tarball"
		ssh oracle@infinit.im "cd www/infinit.im/downloads/$platform && tar xf release.tgz && rm release.tgz"
	)
	rm -rf "$tmp"
done

