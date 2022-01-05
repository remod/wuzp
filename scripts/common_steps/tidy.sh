#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 1 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./tidy.sh BUILD_DIR"
  exit 1
fi

BUILD_DIR=${1}

# Tidy. TODO(remod): Re-enable.
#make -C ${BUILD_DIR} tidy

exit 0
