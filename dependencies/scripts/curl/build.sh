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

uptodate "${CURL_LIBRARIES}" ||
(
    rm -Rf "${BUILDDIR}/${CURL_NAME}"

    download "${CURL_SNAPSHOT}" "${CURL_FINGERPRINT}" "${BUILDDIR}/${CURL_TARBALL}"

    cd "${BUILDDIR}" ||
    die "unable to move to the build directory '${BUILDDIR}'"

    tar xzf "${CURL_TARBALL}" ||
    die "unable to extract the tarball"

    cd "${CURL_NAME}" ||
    die "unable to enter the directory"

    ./configure                                                         \
        --prefix="${WORKDIR}"                                           \
        --with-ssl="${WORKDIR}" ||
    die "unable to configure"

    make install ||
    die "unable to build"
)
