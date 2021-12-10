#!/bin/bash

# Exit immediately if a command fails.
set -ex

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./format.sh SOURCE_DIR BUILD_DIR"
  exit 1
fi

# Format.
make -C ${2} format
git -C ${1} diff --exit-code

exit 0
