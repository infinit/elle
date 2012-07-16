#
# ---------- globals ----------------------------------------------------------
#

# general
PACKAGESDIR="${BASEDIR}/packages"
PLATFORMDIR="${BASEDIR}/platforms/${PLATFORM}"
BUILDDIR="${PLATFORMDIR}/build"
WORKDIR="${PLATFORMDIR}/work"
DEPENDENCIES="openssl qjson"

#
# ---------- imports ----------------------------------------------------------
#

. "${PLATFORMDIR}/common.sh"

#
# ---------- packages ---------------------------------------------------------
#

# openssl
OPENSSL_VERSION="1.0.1c"
OPENSSL_BUILDDIR="openssl-${OPENSSL_VERSION}"
OPENSSL_NAME="openssl-${OPENSSL_VERSION}"
OPENSSL_TARBALL="${OPENSSL_NAME}.tar.gz"
OPENSSL_SNAPSHOT="http://www.openssl.org/source/${OPENSSL_TARBALL}"
OPENSSL_FINGERPRINT="ae412727c8c15b67880aef7bd2999b2e"
OPENSSL_LIBRARIES="${WORKDIR}/lib/libssl.${PLATFORM_LIBRARY_EXTENSION} ${WORKDIR}/lib/libcrypto.${PLATFORM_LIBRARY_EXTENSION}"

# qjson
QJSON_VERSION="0.7.1"
QJSON_BUILDDIR="qjson-${QJSON_VERSION}"
QJSON_NAME="qjson"
QJSON_TARBALL="qjson-${QJSON_VERSION}.tar.bz2"
QJSON_SNAPSHOT="http://downloads.sourceforge.net/project/qjson/qjson/${QJSON_VERSION}/${QJSON_TARBALL}"
QJSON_FINGERPRINT="5a833ad606c164ed8aa69f0873366ace"
QJSON_LIBRARIES="${WORKDIR}/lib/libqjson.${PLATFORM_LIBRARY_EXTENSION}"

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

    TRIES=3

    for i in $(seq 0 ${TRIES}) ; do
        if [ ! -f "${tarball}" ] ; then
            wget "${snapshot}" -O "${tarball}" ||
            die "unable to download the snapshot"
        fi

        checksum=$(openssl dgst -md5 "${tarball}" | cut -d' ' -f2)

        test "${fingerprint}" = "${checksum}" &&
        return

        rm -f "${tarball}"
    done

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
