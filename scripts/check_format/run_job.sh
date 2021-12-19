#!/bin/bash

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_job.sh REPO_DIR BUILD_DIR"
  exit 1
fi

REPO_DIR=${1}
BUILD_DIR=${2}

# These variables do not influence formatting.
C_COMPILER=gcc
CXX_COMPILER=g++
BUILD_TYPE=Debug
UNITS_IMPLEMENTATION=0
VALUE_MATRIX_IMPLEMENTATION=0
SOURCE_DIR=${REPO_DIR}/tests

# Run all steps.
${REPO_DIR}/scripts/check_format/steps/install_dependencies.sh &&
${REPO_DIR}/scripts/common_steps/create_build_environment.sh ${SOURCE_DIR} ${BUILD_DIR} ON ${C_COMPILER} ${CXX_COMPILER} ${BUILD_TYPE} OFF ON OFF ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} &&
${REPO_DIR}/scripts/common_steps/format.sh ${REPO_DIR} ${BUILD_DIR} &&
RETURN_CODE=$?

# Make the host user the owner of the build directory.
chown -R 1000:1000 ${BUILD_DIR}

exit ${RETURN_CODE}
