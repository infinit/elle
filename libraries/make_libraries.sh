#!/bin/sh
# @file make_libraries.sh
# @author <raphael.londeix@gmail.com> Raphael Londeix
# @version @FIXME@

SCRIPT=`basename "$0"`
SCRIPTDIR=`python -c "import os;print os.path.abspath(os.path.dirname('$0'))"`

cd "$SCRIPTDIR"

ARCH=$1

die()
{
	echo $1 1>&2
	exit 1
}

[ -z $ARCH ] && die "You have to specify an architecture"

BUILDDIR="$SCRIPTDIR/$ARCH/build"

mkdir -p "$BUILDDIR"
cd "$SCRIPTDIR/$ARCH"

#############################################################################
# OpenSSL

openssl_version=1.0.0g
openssl_dir=openssl-$openssl_version
openssl_tarball=$openssl_dir.tar.gz
openssl_url="http://www.openssl.org/source/$openssl_tarball"

if [ ! -d $openssl_dir ]
then
	echo "**** Download OpenSSL"
	wget "$openssl_url" -O $openssl_tarball
	tar xzf $openssl_tarball
	rm $openssl_tarball
fi

[ ! -e build/lib/libssl.a -o ! -e build/lib/libcrypto.a ] && (
	echo "**** Build OpenSSL"
	cd $openssl_dir
	./config --prefix="$BUILDDIR"                       \
	         --openssldir="$BUILDDIR/openssl"     \
	         shared no-threads zlib
	make install
	cd -
)

echo "**** OpenSSL up-to-date"

#############################################################################
# msgpack

if [ ! -d msgpack ]
then
	echo "**** Download msgpack"
	git clone git://github.com/msgpack/msgpack.git
else
	echo "**** Update msgpack"
	(
		cd msgpack
		git pull
		cd -
	) 2>&1 > /dev/null
fi

[ ! -e build/lib/libmsgpack.a ] && (
	echo "**** Build msgpack"
	cd msgpack/cpp
	./bootstrap
	./configure --prefix="$BUILDDIR"
	make install
	cd -
)
