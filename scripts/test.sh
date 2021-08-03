#!/bin/bash

# Exit immediately if a command fails.
set -E

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./build.sh TEST_DIR BUILD_CONFIG"
  exit 1
fi

# Test.
# Unfortunately ctest < 3.20 does not support specifying the test directory, so we cd there.
cd ${1}
# If some tests fail, rerun them in verbose mode.
ctest -C ${2} --rerun-failed --output-on-failure
cd -
