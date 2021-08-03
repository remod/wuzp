#!/bin/bash

# Exit immediately if a command fails.
set -E

# Check arguments.
if [ $# != 8 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./create_build_environment.sh SOURCE_DIR BUILD_DIR BUILD_TYPE ENABLE_TEST ENABLE_FORMAT ENABLE_TIDY UNITS_IMPLEMENTATION MATRIX_IMPLEMENTATION"
  exit 1
fi

# Create build environment.
cmake -E make_directory ${2}
cmake -S ${1} -B ${2} -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/gcc -DCMAKE_BUILD_TYPE=${3} -DENABLE_TESTS=${4} -DENABLE_FORMAT=${5} -DENABLE_TIDY=${6} -DUNITS_IMPLEMENTATION=${7} -DMATRIX_IMPLEMENTATION=${8}
