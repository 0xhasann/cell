#!/bin/sh


set -e # Exit early if any commands fail


(
  cd "$(dirname "$0")"
  _log="$(pwd)/build/.cmake-local.log"
  mkdir -p build
  if ! cmake -B build -S . >"$_log" 2>&1; then cat "$_log"; exit 1; fi
  if ! cmake --build ./build >>"$_log" 2>&1; then cat "$_log"; exit 1; fi
)

exec $(dirname "$0")/build/shell "$@"