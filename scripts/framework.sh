#!/usr/bin/env sh

i8warn() {
    echo "[33m*[0m" "$*"
}

i8info() {
    echo "[32m*[0m" "$*"
}

i8error() {
    echo "[31m*[0m" "$*"
}

for file in ${INFINIT_SOURCE_DIR}/scripts/infinit*
do
	. $file
done
