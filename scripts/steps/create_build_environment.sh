#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 8 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./create_build_environment.sh SOURCE_DIR BUILD_DIR BUILD_TYPE ENABLE_TEST ENABLE_FORMAT ENABLE_TIDY UNITS_IMPLEMENTATION MATRIX_IMPLEMENTATION"
  exit 1
fi

# Create build environment.
cmake -E make_directory ${2}
cmake -S ${1} -B ${2} -D CMAKE_C_COMPILER=cc -D CMAKE_CXX_COMPILER=c++ -D CMAKE_BUILD_TYPE=${3} -D ENABLE_TESTS=${4} -D ENABLE_FORMAT=${5} -D ENABLE_TIDY=${6} -D UNITS_IMPLEMENTATION=${7} -D MATRIX_IMPLEMENTATION=${8}

exit 0
