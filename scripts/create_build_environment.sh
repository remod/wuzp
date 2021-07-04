#!/bin/bash

# Exit immediately if a command fails.
set -E

# Check arguments.
if [ $# != 7 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./create_build_environment.sh PATH BUILD_TYPE ENABLE_TEST ENABLE_FORMAT ENABLE_TIDY UNITS_IMPLEMENTATION MATRIX_IMPLEMENTATION"
  exit 1
fi

# Create build environment.
cmake -E make_directory ${1}/build
cmake ${1} -DCMAKE_BUILD_TYPE=${2} -DENABLE_TESTS=${3} -DENABLE_FORMAT=${4} -DENABLE_TIDY=${5} -DUNITS_IMPLEMENTATION=${6} -DMATRIX_IMPLEMENTATION=${7}
