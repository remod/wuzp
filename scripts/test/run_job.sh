#!/bin/bash

# Check arguments.
if [ $# != 7 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_job.sh REPO_DIR BUILD_DIR C_COMPILER CXX_COMPILER BUILD_TYPE UNITS_IMPLEMENTATION VALUE_MATRIX_IMPLEMENTATION"
  exit 1
fi

REPO_DIR=${1}
BUILD_DIR=${2}
C_COMPILER=${3}
CXX_COMPILER=${4}
BUILD_TYPE=${5}
UNITS_IMPLEMENTATION=${6}
VALUE_MATRIX_IMPLEMENTATION=${7}
SOURCE_DIR=${REPO_DIR}/tests

# Run all steps.
${REPO_DIR}/scripts/test/steps/install_dependencies.sh ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} &&
${REPO_DIR}/scripts/test/steps/create_build_environment.sh ${SOURCE_DIR} ${BUILD_DIR} ${C_COMPILER} ${CXX_COMPILER} ${BUILD_TYPE} ON ON ON ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} &&
${REPO_DIR}/scripts/test/steps/build.sh ${BUILD_DIR} ${BUILD_TYPE} &&
${REPO_DIR}/scripts/test/steps/test.sh ${SOURCE_DIR} ${BUILD_DIR} ${BUILD_TYPE} &&
${REPO_DIR}/scripts/test/steps/format.sh ${REPO_DIR} ${BUILD_DIR} &&
${REPO_DIR}/scripts/test/steps/tidy.sh ${BUILD_DIR}
RETURN_CODE=$?

# Make the host user the owner of the build directory.
chown -R 1000:1000 ${BUILD_DIR}

exit ${RETURN_CODE}
