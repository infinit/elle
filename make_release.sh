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

echo "Building Infinit $major.$minor.$rev"

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

	for f in 8infinit \
	         satellites/dictionary/8dictionary \
	         satellites/access/8access \
	         plasma/updater/updater \
	         plasma/installer/installer \
	         plasma/watchdog/watchdog
	do
		[ -x "$build_dir/$f" ] || die "cannot find '$build_dir/$f' binary"
		cp "$build_dir/$f" "$release_dir/binaries" \
			|| die "Cannot copy '$build_dir/$f' to '$release_dir/binaries'"
	done
	python "$SCRIPTDIR"/make_release.py "$release_dir" "$major" "$minor" "$rev"
}


cd build

for dir in *
do
	build_release "$dir" "$release_dir"
done
