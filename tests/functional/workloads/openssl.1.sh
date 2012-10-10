#!/bin/sh

cd "$MNT1"

OPENSSL_VERSION="1.0.1c"
OPENSSL_BUILDDIR="openssl-${OPENSSL_VERSION}"
OPENSSL_NAME="openssl-${OPENSSL_VERSION}"
OPENSSL_TARBALL="${OPENSSL_NAME}.tar.gz"
OPENSSL_SNAPSHOT="http://www.openssl.org/source/${OPENSSL_TARBALL}"
OPENSSL_FINGERPRINT="ae412727c8c15b67880aef7bd2999b2e"

fatal()
{
	echo "FATAL: ${1}" 1>&2
	exit 1
}

download()
{
	snapshot="${1}"
	fingerprint="${2}"
	tarball="${3}"

	TRIES=3

	for i in $(seq 0 ${TRIES})
	do
		if [ ! -f "${tarball}" ]
		then
			wget "${snapshot}" -O "${tarball}" ||
				fatal "unable to download the snapshot"
		fi

		checksum=$(openssl dgst -md5 "${tarball}" | cut -d' ' -f2)

		test "${fingerprint}" = "${checksum}" && return

		rm -f "${tarball}"
	done

	fatal "the checksum differs for '${tarball}'"
}

download "${OPENSSL_SNAPSHOT}" "${OPENSSL_FINGERPRINT}" "${OPENSSL_TARBALL}"

tar xf "${OPENSSL_TARBALL}" || fatal "unable to extract the tarball"

cd "${OPENSSL_NAME}" || fatal "unable to enter the directory"

./config shared || fatal "unable to configure"

make || fatal "unable to build"

