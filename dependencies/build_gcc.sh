#!/bin/sh
# @file make_gcc.sh
# @author <raphael.londeix@gmail.com> Raphael Londeix
# @version @FIXME@

SCRIPT=`basename "$0"`
SCRIPTDIR=`python -c "import os;print os.path.abspath(os.path.dirname('$0'))"`

GCC_RELEASE_NAME=gcc_4_6_3
GCC_SVN="svn://gcc.gnu.org/svn/gcc/tags/${GCC_RELEASE_NAME}_release/"

[ ! -d gcc ] && mkdir gcc

cd gcc

die()
{
	echo "$1" 1>&2
	exit 1
}

if [ ! -d ${GCC_RELEASE_NAME} ]
then
	svn co ${GCC_SVN} ${GCC_RELEASE_NAME} || die "Cannot checkout gcc svn"
else
	echo "Updating gcc"
	( cd ${GCC_RELEASE_NAME} && svn up ) || die "Cannot update gcc svn"
fi

cd ${GCC_RELEASE_NAME}

if [ ! -d mpc ]
then
	wget "http://www.multiprecision.org/mpc/download/mpc-0.9.tar.gz" -O mpc.tgz
	tar xf mpc.tgz
	mv mpc-0.9 mpc
fi

if [ ! -d mpfr ]
then
	wget "http://www.mpfr.org/mpfr-current/mpfr-3.1.0.tar.bz2" -O mpfr.tbz
	tar xf mpfr.tbz
	mv mpfr-3.1.0 mpfr
fi

if [ ! -d gmp ]
then
	wget "ftp://ftp.gmplib.org/pub/gmp-5.0.4/gmp-5.0.4.tar.bz2" -O gmp.tbz
	tar xf gmp.tbz
	mv gmp-5.0.4 gmp
fi

cd ..

mkdir -p ${GCC_RELEASE_NAME}-build
cd ${GCC_RELEASE_NAME}-build


../${GCC_RELEASE_NAME}/configure -v                             \
	--with-pkgversion='Ubuntu LTS 10.04'                        \
	--enable-languages=c,c++                                    \
	--prefix=/usr                                               \
	--enable-shared                                             \
	--enable-linker-build-id                                    \
	--with-system-zlib                                          \
	--libexecdir=/usr/lib                                       \
	--without-included-gettext                                  \
	--enable-threads=posix                                      \
	--with-gxx-include-dir=/usr/include/c++/4.4                 \
	--program-suffix=-4.4                                       \
	--enable-nls                                                \
	--enable-clocale=gnu                                        \
	--enable-libstdcxx-debug                                    \
	--enable-plugin                                             \
	--enable-objc-gc                                            \
	--disable-werror                                            \
	--with-arch-32=i486                                         \
	--with-tune=generic                                         \
	--enable-checking=release                                   \
	--build=x86_64-linux-gnu                                    \
	--host=x86_64-linux-gnu                                     \
	--target=x86_64-linux-gnu                                   \
	--disable-multilib                                          \
|| die "Couldn't configure gcc!"

make
echo "type 'cd gcc/${GCC_RELEASE_NAME} && sudo make install' to install it !"
