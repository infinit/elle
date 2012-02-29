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

. "${BASEDIR}/environment.sh"
. "${BASEDIR}/common.sh"

#
# ---------- entry point ------------------------------------------------------
#

uptodate "${JSONCPP_LIBRARIES}" ||
(
    rm -Rf "${BUILDDIR}/${JSONCPP_NAME}"

    download "${JSONCPP_SNAPSHOT}" "${JSONCPP_FINGERPRINT}" "${BUILDDIR}/${JSONCPP_TARBALL}"

    cd "${BUILDDIR}" ||
    die "unable to move to the build directory '${BUILDDIR}'"

    tar xzf "${JSONCPP_TARBALL}" ||
    die "unable to extract the tarball"

    cd "${JSONCPP_NAME}" ||
    die "unable to enter the directory"

    scons platform=linux-gcc ||
    die "unable to configure"

    cp buildscons/linux-gcc-*/src/lib_json/libjson_linux-gcc-*_libmt.a "${WORKDIR}/lib/libjsoncpp.a"
    cp buildscons/linux-gcc-*/src/lib_json/libjson_linux-gcc-*_libmt.so "${WORKDIR}/lib/libjsoncpp.so"
    cp buildscons/linux-gcc-*/src/lib_json/libjson_linux-gcc-*_libmt.dylib "${WORKDIR}/lib/libjsoncpp.dylib"

    cp -R include/json "${WORKDIR}/include/jsoncpp"
)
