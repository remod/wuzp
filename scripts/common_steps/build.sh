#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./build.sh BUILD_DIR BUILD_CONFIG"
  exit 1
fi

BUILD_DIR=${1}
BUILD_CONFIG=${2}

# Build.
cmake --build ${BUILD_DIR} --config ${BUILD_CONFIG} --parallel

exit 0
