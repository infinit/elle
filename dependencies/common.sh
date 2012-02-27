#
# ---------- header -----------------------------------------------------------
#
# project       libraries
#
# license       infinit
#
# author        julien quintard   [fri feb 24 15:13:21 2012]
#

#
# ---------- globals ----------------------------------------------------------
#

# general
SCRIPTSDIR="${BASEDIR}/scripts"
PLATFORMDIR="${BASEDIR}/platforms/${PLATFORM}"
BUILDDIR="${BASEDIR}/build"
WORKDIR="${BASEDIR}/work"
DEPENDENCIES="openssl msgpack jsoncpp curl qjson"

# openssl
OPENSSL_VERSION="1.0.0g"
OPENSSL_BUILDDIR="openssl-${OPENSSL_VERSION}"
OPENSSL_NAME="openssl-${OPENSSL_VERSION}"
OPENSSL_TARBALL="${OPENSSL_NAME}.tar.gz"
OPENSSL_SNAPSHOT="http://www.openssl.org/source/${OPENSSL_TARBALL}"
OPENSSL_FINGERPRINT="07ecbe4324f140d157478637d6beccf1"
OPENSSL_LIBRARIES="${WORKDIR}/lib/libssl.a ${WORKDIR}/lib/libcrypto.a"

# msgpack
MSGPACK_VERSION="0.5.7"
MSGPACK_BUILDDIR="msgpack-${MSGPACK_VERSION}"
MSGPACK_NAME="msgpack-${MSGPACK_VERSION}"
MSGPACK_TARBALL="${MSGPACK_NAME}.tar.gz"
MSGPACK_SNAPSHOT="http://msgpack.org/releases/cpp/${MSGPACK_TARBALL}"
MSGPACK_FINGERPRINT="705106a9378c792fe22d285dba5c142c"
MSGPACK_LIBRARIES="${WORKDIR}/lib/libmsgpack.a"

# jsoncpp
JSONCPP_VERSION="0.5.0"
JSONCPP_BUILDDIR="jsoncpp-src-${JSONCPP_VERSION}"
JSONCPP_NAME="jsoncpp-src-${JSONCPP_VERSION}"
JSONCPP_TARBALL="${JSONCPP_NAME}.tar.gz"
JSONCPP_SNAPSHOT="http://downloads.sourceforge.net/project/jsoncpp/jsoncpp/${JSONCPP_VERSION}/${JSONCPP_TARBALL}"
JSONCPP_FINGERPRINT="24482b67c1cb17aac1ed1814288a3a8f"
JSONCPP_LIBRARIES="${WORKDIR}/lib/libjsoncpp.a"

# curl
CURL_VERSION="7.24.0"
CURL_BUILDDIR="curl-${CURL_VERSION}"
CURL_NAME="curl-${CURL_VERSION}"
CURL_TARBALL="${CURL_NAME}.tar.gz"
CURL_SNAPSHOT="http://curl.haxx.se/download/${CURL_TARBALL}"
CURL_FINGERPRINT="b93420f80a2baaa61a0f45214eddc2ba"
CURL_LIBRARIES="${WORKDIR}/lib/libcurl.a"

# qjson
QJSON_VERSION="0.7.1"
QJSON_BUILDDIR="qjson-${QJSON_VERSION}"
QJSON_NAME="qjson"
QJSON_TARBALL="qjson-${QJSON_VERSION}.tar.bz2"
QJSON_SNAPSHOT="http://downloads.sourceforge.net/project/qjson/qjson/${QJSON_VERSION}/${QJSON_TARBALL}"
QJSON_FINGERPRINT="5a833ad606c164ed8aa69f0873366ace"
QJSON_LIBRARIES=""

#
# ---------- imports ----------------------------------------------------------
#

if [ -d "${PLATFORMDIR}" ] ; then
    source "${PLATFORMDIR}/common.sh"
fi

#
# ---------- functions --------------------------------------------------------
#

die()
{
    echo "[!] ${1}" 1>&2
    exit 1
}

download()
{
    snapshot="${1}"
    fingerprint="${2}"
    tarball="${3}"

    if [ ! -f "${tarball}" ] ; then
        wget "${snapshot}" -O "${tarball}" ||
        die "unable to download the snapshot"
    fi

    checksum=$(openssl dgst -md5 "${tarball}" | cut -d' ' -f2)

    test "${fingerprint}" = "${checksum}" ||
    die "the checksum differs for '${tarball}'"
}

uptodate()
{
    libraries="${1}"

    for library in ${libraries} ; do
        if [ ! -f "${library}" ] ; then
            return 1
        fi
    done

    return 0
}
