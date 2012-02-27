#!/bin/sh
# @file make_release.sh
# @author <raphael.londeix@gmail.com> Raphael Londeix
# @version @FIXME@

SCRIPT=`basename "$0"`
SCRIPTDIR=`python -c "import os;print os.path.abspath(os.path.dirname('$0'))"`

cd "$SCRIPTDIR"

die()
{
	echo "$1" 1>&2
	exit 1
}

[ ! -d build ] && die "You have to build the project into build/ directory"

rev=`git rev-parse --short HEAD`
major=`cat CMakeLists.txt| grep INFINIT_VERSION_MAJOR | head -n1 | cut -d\" -f2`
minor=`cat CMakeLists.txt| grep INFINIT_VERSION_MINOR | head -n1 | cut -d\" -f2`
release_dir="$SCRIPTDIR/releases/release-$rev"
server_arch=linux64

echo "==== Building Infinit $major.$minor.$rev"

mkdir -p "$release_dir"
rm -rf "$release_dir"
mkdir -p "$release_dir"

[ ! -d "$release_dir" ] && die "Cannot create '$release_dir' directory"


build_release()
{
	local build_dir release_dir
	build_dir="$1"
	release_dir="$2/$1"
	mkdir -p "$release_dir/binaries"
	[ ! -d "$release_dir/binaries" ] && die "Cannot create '$release_dir/binaries' directory"

	echo "== Copying $build_dir files"
	for f in 8infinit \
	         satellites/dictionary/8dictionary \
	         satellites/access/8access \
	         plasma/updater/updater \
	         plasma/installer/installer \
	         plasma/watchdog/watchdog
	do
		[ -x "$build_dir/$f" ] || die "cannot find '$build_dir/$f' binary"
		echo "= Copying '$build_dir/$f' binary"
		cp "$build_dir/$f" "$release_dir/binaries" \
			|| die "Cannot copy '$build_dir/$f' to '$release_dir/binaries'"
	done
	echo "== Stripping $build_dir files"
	for bin in "$release_dir/binaries"/*
	do
		echo "= Stripping '`basename "$bin"`'"
		strip -s "$bin"
	done
	echo "== Writing XML release file of $build_dir platform"
	python "$SCRIPTDIR"/make_release.py "$release_dir" "$major" "$minor" "$rev"
}


cd build

for dir in *
do
	echo "=== Prepare release for $dir platform"
	cmake "$dir"
	make --quiet -C "$dir"
	build_release "$dir" "$release_dir/downloads"
done

if [ ! -d "$server_arch" ]
then
	restart_server=0
	echo "Server architecture need $server_arch build dir"
else
	restart_server=1
	echo "==== Prepare server release ($server_arch)"

	for f in creosus                        \
	         creosus-server                 \
	         meta                           \
	         meta-server                    \
	         meta-tests                     \
	         pythia                         \
	         metalib                        \
	         troll                          \
	         production-meta-server.sh      \
	         production-creosus-server.sh
	do
		echo "=== Copying '$f' to 'release-$rev/'"
		cp -r "$server_arch/oracle/disciples/$f" "$release_dir/"
	done

	clean_release()
	{
		echo "==== Cleaning release-$rev/ directory"
		find "$1" -name 'CMakeFiles' | xargs rm -rf
		find "$1" \(                    \
		        -name '*.pyc' -or       \
		        -name '*.o' -or         \
		        -name '*.cmake' -or     \
		        -name '*.marks' -or     \
		        -name 'Makefile'        \
		\) -delete 2>&1 > /dev/null
	}

	clean_release "$release_dir"

	cd "$release_dir"
	ln -s "creosus/static" .
	cd - > /dev/null
fi

which tree > /dev/null
[ $? = 0 ] && tree -h "$release_dir"

release_tarball="$release_dir.tgz"
echo "==== Creating `basename $release_tarball`"
(
	cd "$release_dir" &&
	tar cjf "$release_tarball" *
) || die "Cannot create tarball $release_tarball"

size=`du -hs "$release_tarball" | cut -f1`
echo "==== Uploading tarball ($size)"
scp "$release_tarball" infinit.im:/usr/local/www/infinit.im/

echo "==== Deploying tarball"
ssh oracle@infinit.im "cd www/infinit.im && tar xf `basename "$release_tarball"` && rm `basename "$release_tarball"`"

if [ $restart_server = 1 ]
then
	echo "==== Restarting servers"
	#ssh oracle@infinit.im sh www/infinit.im/production-meta-server.sh
	#ssh oracle@infinit.im sh www/infinit.im/production-creosus-server.sh
fi
