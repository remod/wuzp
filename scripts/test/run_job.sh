#!/bin/bash

# Check arguments.
if [ $# != 9 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_job.sh REPO_DIR BUILD_DIR ENABLE_CODE_COVERAGE CODE_COVERAGE_DIR CMAKE_C_COMPILER CMAKE_CXX_COMPILER BUILD_TYPE UNITS_IMPLEMENTATION VALUE_MATRIX_IMPLEMENTATION"
  exit 1
fi

REPO_DIR=${1}
BUILD_DIR=${2}
ENABLE_CODE_COVERAGE=${3}
CODE_COVERAGE_DIR=${4}
CMAKE_C_COMPILER=${5}
CMAKE_CXX_COMPILER=${6}
BUILD_TYPE=${7}
UNITS_IMPLEMENTATION=${8}
VALUE_MATRIX_IMPLEMENTATION=${9}
SOURCE_DIR=${REPO_DIR}/tests

# Run all steps.
${REPO_DIR}/scripts/common_steps/upgrade.sh &&
${REPO_DIR}/scripts/test/steps/install_dependencies.sh ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} &&
${REPO_DIR}/scripts/common_steps/create_build_environment.sh ${SOURCE_DIR} ${BUILD_DIR} ON ${CMAKE_C_COMPILER} ${CMAKE_CXX_COMPILER} ${BUILD_TYPE} ON OFF ON ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} &&
${REPO_DIR}/scripts/common_steps/build.sh ${BUILD_DIR} ${BUILD_TYPE} &&
${REPO_DIR}/scripts/common_steps/test.sh ${BUILD_DIR} ${BUILD_TYPE} &&
${REPO_DIR}/scripts/common_steps/generate_code_coverage.sh ${ENABLE_CODE_COVERAGE} ${REPO_DIR} ${BUILD_DIR} ${CODE_COVERAGE_DIR} &&
${REPO_DIR}/scripts/common_steps/tidy.sh ${BUILD_DIR}
RETURN_CODE=$?

# Make the host user the owner of the build directory.
chown -R 1000:1000 ${BUILD_DIR}

exit ${RETURN_CODE}
