#!/bin/sh
# @file make_libraries.sh
# @author <raphael.londeix@gmail.com> Raphael Londeix
# @version @FIXME@

SCRIPT=`basename "$0"`
SCRIPTDIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`

cd "$SCRIPTDIR"

ARCH=$1

die()
{
	echo $1 1>&2
	exit 1
}

[ -z $ARCH ] && die "You have to specify an architecture"
[ -f "$SCRIPTDIR/$SCRIPT" ] || die "Wrong value for SCRIPTDIR: '$SCRIPTDIR'"

BUILDDIR="$SCRIPTDIR/$ARCH/build"

if [ ! -d "$BUILDDIR" ]
then
	mkdir -p "$BUILDDIR" || die "Cannot create $BUILDDIR directory"
fi

cd "$SCRIPTDIR/$ARCH" || die "Cannot cd to $SCRIPTDIR/$ARCH directory"
#############################################################################
# OpenSSL

openssl_version=1.0.0g
openssl_dir=openssl-$openssl_version
openssl_tarball=$openssl_dir.tar.gz
openssl_url="http://www.openssl.org/source/$openssl_tarball"

if [ ! -d $openssl_dir ]
then
	(
		echo "**** Download OpenSSL" && \
		wget "$openssl_url" -O $openssl_tarball && \
		tar xzf $openssl_tarball && \
		rm $openssl_tarball
	) || die "Couldn't checkout openssl"
fi

if [ ! -e build/lib/libssl.a -o ! -e build/lib/libcrypto.a ]
then
	(
		echo "**** Build OpenSSL"
		cd $openssl_dir && \
		./config --prefix="$BUILDDIR"                       \
		    --openssldir="$BUILDDIR/openssl" && \
		make install && \
		cd -
	) || die "Couldn't build openssl"
fi
echo "**** OpenSSL up-to-date"

#############################################################################
# msgpack

msgpack_version=0.5.7
msgpack_dir=msgpack-$msgpack_version
msgpack_tarball=$msgpack_dir.tar.gz
msgpack_url="http://msgpack.org/releases/cpp/$msgpack_tarball"

if [ ! -d $msgpack_dir ]
then
	(
		echo "**** Download msgpack" && \
		wget "$msgpack_url" -O $msgpack_tarball && \
		tar xzf $msgpack_tarball && \
		rm $msgpack_tarball
	) || die "Couldn't checkout msgpack"
fi

if [ ! -e build/lib/libmsgpack.a ]
then
	(
		echo "**** Build msgpack" && \
		cd $msgpack_dir && \
		./configure --prefix="$BUILDDIR" CFLAGS="${CFLAGS} -fPIC" && \
		make install && \
		cd -
	) || die "Couldn't build msgpack"
fi

	echo "**** msgpack up-to-date"

	#############################################################################
	# jsoncpp

if [ ! -d jsoncpp ]
then
	(
		echo "**** Download jsoncpp" && \
		svn co https://jsoncpp.svn.sourceforge.net/svnroot/jsoncpp/trunk/jsoncpp jsoncpp
	) || die "Couldn't checkout jsoncpp"
fi

if [ ! -e build/lib/libjson.a ]
then
	(
		echo "**** Build jsoncpp" && \
		cd jsoncpp && \
		scons platform=linux-gcc && \
		cd - && \
		cp jsoncpp/buildscons/*/src/lib_json/*.a build/lib/libjson.a && \
		cp jsoncpp/buildscons/*/src/lib_json/*.so build/lib/libjson.so && \
		cp -r jsoncpp/include/json build/include/jsoncpp
	) || die "Could'nt build jsoncpp"
fi

echo "**** jsoncpp up-to-date"

#############################################################################
# curl

curl_version=7.24.0
curl_dir=curl-$curl_version
curl_tarball=$curl_dir.tar.gz
curl_url="http://curl.haxx.se/download/$curl_tarball"

if [ ! -d $curl_dir ]
then
	(
		echo "**** Download curl" && \
		wget "$curl_url" -O $curl_tarball && \
		tar xzf $curl_tarball && \
		rm $curl_tarball
	) || die "Couldn't checkout curl"
fi


if [ ! -e build/lib/libcurl.a ]
then
	(
		cd $curl_dir && \
		./configure --prefix="$BUILDDIR" --with-ssl="$BUILDDIR/openssl" && \
		make install
	) || die "Couldn't build curl"
fi

	echo "**** curl up-to-date"


	#############################################################################
	# qjson

if [ ! -d qjson ]
then
	(
		echo "**** Download qjson" && \
		git clone git://gitorious.org/qjson/qjson.git
	) || die "Couldn't checkout qjson"
fi

if [ -z "`ls build/lib/libqjson*`" ]
then
	(
		echo "**** build qjson" && \
		cd qjson && \
		mkdir build && \
		cd build && \
		cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX="$BUILDDIR" .. && \
		make install && \
		cd ../.. && \
		mkdir -p build/include/QJson && \
		cd build/include/QJson && \
		echo '#include "../qjson/serializer.h"' > Serializer && \
		echo '#include "../qjson/parser.h"' > Parser && \
		echo '#include "../qjson/qobjecthelper.h"' > QObjectHelper
	) || die "Couldn't build qjson"
fi
echo "**** qjson up-to-date"
