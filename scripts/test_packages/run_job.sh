#!/bin/bash

# Check arguments.
if [ $# != 8 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_job.sh REPO_DIR PACKAGE_DIR BUILD_DIR C_COMPILER CXX_COMPILER BUILD_TYPE UNITS_IMPLEMENTATION VALUE_MATRIX_IMPLEMENTATION"
  exit 1
fi

REPO_DIR=${1}
PACKAGE_DIR=${2}
BUILD_DIR=${3}
C_COMPILER=${4}
CXX_COMPILER=${5}
BUILD_TYPE=${6}
UNITS_IMPLEMENTATION=${7}
VALUE_MATRIX_IMPLEMENTATION=${8}
SOURCE_DIR=${REPO_DIR}/tests

# Run all steps.
${REPO_DIR}/scripts/test_packages/steps/install_dependencies.sh ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} &&
${REPO_DIR}/scripts/test_packages/steps/install_packages.sh ${PACKAGE_DIR} ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} &&
${REPO_DIR}/scripts/common_steps/create_build_environment.sh ${SOURCE_DIR} ${BUILD_DIR} OFF ${C_COMPILER} ${CXX_COMPILER} ${BUILD_TYPE} ON ON ON ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} &&
${REPO_DIR}/scripts/common_steps/build.sh ${BUILD_DIR} ${BUILD_TYPE} &&
${REPO_DIR}/scripts/common_steps/test.sh ${BUILD_DIR} ${BUILD_TYPE}
RETURN_CODE=$?

# Make the host user the owner of the build directory.
chown -R 1000:1000 ${BUILD_DIR}

exit ${RETURN_CODE}
