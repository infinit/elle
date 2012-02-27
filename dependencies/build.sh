# /bin/bash
#
# ---------- header -----------------------------------------------------------
#
# project       libraries
#
# license       infinit
#
# author        julien quintard   [fri feb 24 14:56:04 2012]
#

#
# ---------- globals ----------------------------------------------------------
#

SCRIPT=`basename "${0}"`
ROOTDIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`

PLATFORM="${1}"

#
# ---------- imports ----------------------------------------------------------
#

BASEDIR="${ROOTDIR}"

source "${BASEDIR}/common.sh"

#
# ---------- entry point ------------------------------------------------------
#

#
# check the arguments and environment.
#
[ -z ${PLATFORM} ] && die "usage: ${SCRIPT} [architecture: linux32/macosx64/etc."
[ -f "${ROOTDIR}/${SCRIPT}" ] || die "invalid value for ROOTDIR: '${ROOTDIR}'"

if [ ! -d "${BUILDDIR}" ] ; then
    mkdir -p "${BUILDDIR}" || die "unable to create the '${BUILDDIR}' directory"
fi

if [ ! -d "${WORKDIR}" ] ; then
    mkdir -p "${WORKDIR}" || die "unable to create the '${WORKDIR}' directory"
fi

#
# create the environment.
#
cat >"${ROOTDIR}/environment.sh" <<EOF
ROOTDIR="${ROOTDIR}"
PLATFORM="${PLATFORM}"
PLATFORMDIR="${PLATFORMDIR}"
BUILDDIR="${BUILDDIR}"
WORKDIR="${WORKDIR}"
EOF

#
# go through the dependencies and build them.
#
# note that, if a platform-specific build script is found, it is used.
# otherwise, the default build script is used.
#
for dependency in ${DEPENDENCIES} ; do
    echo "----------[ ${dependency}"

    if [ -f "${PLATFORMDIR}/${dependency}/build.sh" ] ; then
        ${PLATFORMDIR}/${dependency}/build.sh || exit 1
    else
        ${SCRIPTSDIR}/${dependency}/build.sh || exit 1
    fi
done
