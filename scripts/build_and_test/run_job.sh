#!/bin/bash

# Check arguments.
if [ $# != 8 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_job.sh SOURCE_DIR BUILD_DIR PACKAGE_DIR C_COMPILER CXX_COMPILER BUILD_TYPE UNITS_IMPLEMENTATION VALUE_MATRIX_IMPLEMENTATION"
  exit 1
fi

SOURCE_DIR=${1}
BUILD_DIR=${2}
PACKAGE_DIR=${3}
C_COMPILER=${4}
CXX_COMPILER=${5}
BUILD_TYPE=${6}
UNITS_IMPLEMENTATION=${7}
VALUE_MATRIX_IMPLEMENTATION=${8}

# Run all steps.
${SOURCE_DIR}/scripts/build_and_test/steps/install_dependencies.sh ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} &&
${SOURCE_DIR}/scripts/build_and_test/steps/create_build_environment.sh ${SOURCE_DIR} ${BUILD_DIR} ${C_COMPILER} ${CXX_COMPILER} ${BUILD_TYPE} ON ON ON ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} &&
${SOURCE_DIR}/scripts/build_and_test/steps/build.sh ${BUILD_DIR} ${BUILD_TYPE} &&
${SOURCE_DIR}/scripts/build_and_test/steps/test.sh ${SOURCE_DIR} ${BUILD_DIR} ${BUILD_TYPE} &&
${SOURCE_DIR}/scripts/build_and_test/steps/format.sh ${SOURCE_DIR} ${BUILD_DIR} &&
${SOURCE_DIR}/scripts/build_and_test/steps/tidy.sh ${BUILD_DIR}
RETURN_CODE=$?

# Make the host user the owner of the build directory.
chown -R 1000:1000 ${BUILD_DIR}

exit ${RETURN_CODE}
