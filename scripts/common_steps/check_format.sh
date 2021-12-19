#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./check_format.sh REPO_DIR BUILD_DIR"
  exit 1
fi

REPO_DIR=${1}
BUILD_DIR=${2}

# Format.
make -C ${BUILD_DIR} format
git -C ${REPO_DIR} diff --exit-code

exit 0
