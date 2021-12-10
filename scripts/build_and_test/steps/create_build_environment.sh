#!/bin/bash

# Exit immediately if a command fails.
set -ex

# Check arguments.
if [ $# != 10 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./create_build_environment.sh SOURCE_DIR BUILD_DIR C_COMPILER CXX_COMPILER BUILD_TYPE ENABLE_TEST ENABLE_FORMAT ENABLE_TIDY UNITS_IMPLEMENTATION VALUE_MATRIX_IMPLEMENTATION"
  exit 1
fi

# Create build environment.
cmake -E make_directory ${2}
cmake -S ${1} -B ${2} -D CMAKE_C_COMPILER=${3} -D CMAKE_CXX_COMPILER=${4} -D CMAKE_BUILD_TYPE=${5} -D ENABLE_TESTS=${6} -D ENABLE_FORMAT=${7} -D ENABLE_TIDY=${8} -D UNITS_IMPLEMENTATION=${9} -D VALUE_MATRIX_IMPLEMENTATION=${10} -D CODE_COVERAGE=ON

exit 0
