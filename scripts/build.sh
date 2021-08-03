#!/bin/bash

# Exit immediately if a command fails.
set -E

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./build.sh BUILD_DIR BUILD_CONFIG"
  exit 1
fi

# Build.
cmake --build ${1} --config ${2}
