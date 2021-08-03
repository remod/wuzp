#!/bin/bash

# Exit immediately if a command fails.
set -E

# Check arguments.
if [ $# != 1 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./format.sh BUILD_DIR"
  exit 1
fi

# Format.
make -C ${1} format
exit git -C ${1} diff --exit-code
