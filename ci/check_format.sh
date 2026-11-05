#!/usr/bin/env bash
# Checks the code format with the clang-format version pinned in the dev container.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

# The pre-commit hook and the dev container must pin the same clang-format version.
IMAGE_VERSION=$(sed -n 's/^ARG CLANG_FORMAT_VERSION=//p' .devcontainer/Dockerfile)
HOOK_VERSION=$(sed -n 's/^ *rev: v//p' .pre-commit-config.yaml)
if [[ ${IMAGE_VERSION} != "${HOOK_VERSION}" ]]; then
  echo "clang-format versions differ: ${IMAGE_VERSION} (.devcontainer/Dockerfile) vs ${HOOK_VERSION} (.pre-commit-config.yaml)." >&2
  exit 1
fi

clang-format --version
find examples tests/src wuzp*/include -name '*.[ch]pp' -print0 | xargs -0 clang-format --dry-run --Werror --style=file
echo "The code is formatted correctly."
