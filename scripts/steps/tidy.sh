#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 1 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./format.sh BUILD_DIR"
  exit 1
fi

# Tidy.
make -C ${1} tidy

exit 0
