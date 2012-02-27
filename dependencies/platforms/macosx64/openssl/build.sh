#
# ---------- header -----------------------------------------------------------
#
# project       libraries
#
# license       infinit
#
# author        julien quintard   [fri feb 24 14:55:50 2012]
#

#
# ---------- information ------------------------------------------------------
#
# use:
#   port fetch openssl
#   port extract openssl
#   port patch openssl
#
# at this point, the local repository will be ready to get configured and
# built.
#
# this local repository can be found at:
#
#   /opt/local/var/macports/sources/rsync.macports.org/release/tarballs/ports/devel/openssl/
#

#
# ---------- globals ----------------------------------------------------------
#

SCRIPT=`basename "${0}"`
ROOTDIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`

#
# ---------- imports ----------------------------------------------------------
#

BASEDIR="${ROOTDIR}/../../.."

source "${BASEDIR}/environment.sh"
source "${BASEDIR}/common.sh"

#
# ---------- entry point ------------------------------------------------------
#

FILESDIR="${PLATFORMDIR}/openssl/files"

uptodate "${OPENSSL_LIBRARIES}" ||
(
    rm -Rf "${BUILDDIR}/${OPENSSL_NAME}"

    download "${OPENSSL_SNAPSHOT}" "${OPENSSL_FINGERPRINT}" "${BUILDDIR}/${OPENSSL_TARBALL}"

    cd "${BUILDDIR}" ||
    die "unable to move to the build directory '${BUILDDIR}'"

    tar xzf "${OPENSSL_TARBALL}" ||
    die "unable to extract the tarball"

    cp "${FILESDIR}/Configure" "${BUILDDIR}/${OPENSSL_NAME}" ||
    die "unable to copy the file"

    cp "${FILESDIR}/Makefile.org" "${BUILDDIR}/${OPENSSL_NAME}" ||
    die "unable to copy the file"

    cp "${FILESDIR}/doc/crypto/bn_internal.pod" "${BUILDDIR}/${OPENSSL_NAME}/doc/crypto" ||
    die "unable to copy the file"

    cp "${FILESDIR}/crypto/bf/Makefile" "${BUILDDIR}/${OPENSSL_NAME}/crypto/bf" ||
    die "unable to copy the file"

    cd "${OPENSSL_NAME}" ||
    die "unable to enter the directory"

    ./Configure                                                         \
        --prefix="${WORKDDIR}"                                          \
	--openssldir="${WORKDIR}"                                       \
	zlib no-asm no-krb5 shared darwin64-x86_64-cc ||
    die "unable to configure"

    make install ||
    die "unable to build"
)
