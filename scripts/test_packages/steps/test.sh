#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 3 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./test.sh BUILD_DIR BUILD_CONFIG"
  exit 1
fi

BUILD_DIR=${1}
BUILD_CONFIG=${2}

# Test.
# Unfortunately ctest < 3.20 does not support specifying the test directory, so we cd there.
cd ${BUILD_DIR}
# If some tests fail, rerun them in verbose mode.
ctest -C ${BUILD_CONFIG} --rerun-failed --output-on-failure
cd -

exit 0
