#!/bin/sh

#
# ---------- information ------------------------------------------------------
#
# this script is responsible for locally building the dependency libraries.
#
# the script goes through all the dependencies all call a script dedicated
# to building the library. first the script looks for a platform-specific
# script, for example platforms/macosx64/packages/openssl/build.sh would
# be used, if present. otherwise, the system relies on the default building
# script, for example packages/openssl/build.sh.
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

. "${BASEDIR}/common.sh"

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
    if [ -f "${PLATFORMDIR}/packages/${dependency}/build.sh" ] ; then
        ${PLATFORMDIR}/packages/${dependency}/build.sh || exit 1
    else
        ${PACKAGESDIR}/${dependency}/build.sh || exit 1
    fi

    echo "[ dep] Built target ${dependency}"
done
