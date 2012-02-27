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
# ---------- globals ----------------------------------------------------------
#

SCRIPT=`basename "${0}"`
ROOTDIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`

#
# ---------- imports ----------------------------------------------------------
#

BASEDIR="${ROOTDIR}/../.."

source "${BASEDIR}/environment.sh"
source "${BASEDIR}/common.sh"

#
# ---------- entry point ------------------------------------------------------
#

uptodate "${OPENSSL_LIBRARIES}" ||
(
    rm -Rf "${BUILDDIR}/${OPENSSL_NAME}"

    download "${OPENSSL_SNAPSHOT}" "${OPENSSL_FINGERPRINT}" "${BUILDDIR}/${OPENSSL_TARBALL}"

    cd "${BUILDDIR}" ||
    die "unable to move to the build directory '${BUILDDIR}'"

    tar xzf "${OPENSSL_TARBALL}" ||
    die "unable to extract the tarball"

    cd "${OPENSSL_NAME}" ||
    die "unable to enter the directory"

    ./config                                                            \
        --prefix="${WORKDDIR}"                                          \
        --openssldir="${WORKDIR}"                                       \
        shared                                                          \
    || die "unable to configure"

    make install ||
    die "unable to build"
)
