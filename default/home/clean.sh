#!/bin/sh

ROOT=$(dirname "$0")

error ()
{
    for line; do
        echo 1>&2 "$line"
    done
    exit 1
}

run ()
{
    echo "Running: $@"
    "$@"
}

echo -n "Cleanup networks in $ROOT/.config/infinit (y/n) ? "
read CONFIRM

case "$CONFIRM" in
    (y*|Y*)
        ;;
    (*)
        error "Aborted"
        ;;
esac

cd "$ROOT"
run git checkout HEAD "$ROOT/.config"
for network in "$ROOT"/.config/infinit/networks/*; do
    run git clean -fd $network/shelter
done
