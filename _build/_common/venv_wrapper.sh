#!/bin/bash

# Should arguably be made part of drake itself?

set -e

ARCH_DIR="$(readlink -f "$(dirname $0)")"

if [[ "$ELLE_BUILD_USE_VENV" ]]; then
  VENV_DIR="$ARCH_DIR/venv"
  [ -d "$VENV_DIR" ] || python3 -m venv "$VENV_DIR"

  . "$VENV_DIR/bin/activate"

  REPO_ROOT=$(git rev-parse --show-toplevel)
  REQS_FILE="$REPO_ROOT/drake/requirements.txt"
  LAST_INSTALL_SENTINEL="$ARCH_DIR/last_venv_install"

  if [[ "$REQS_FILE" -nt "$LAST_INSTALL_SENTINEL" ]]; then
    pip install -r "$REQS_FILE"
    touch "$LAST_INSTALL_SENTINEL"
  fi
fi

exec "$ARCH_DIR/run" "$@"
