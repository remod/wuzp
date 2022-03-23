#!/bin/bash

# Check arguments.
if [ $# != 3 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./run_job.sh REPO_DIR WRITABLE_REPO_DIR BUILD_DIR"
  exit 1
fi

REPO_DIR=${1}
WRITABLE_REPO_DIR=${2}
BUILD_DIR=${3}

# These variables do not influence formatting.
CMAKE_C_COMPILER=gcc
CMAKE_CXX_COMPILER=g++
BUILD_TYPE=Debug
UNITS_IMPLEMENTATION=0
VALUES_IMPLEMENTATION=0
SOURCE_DIR=${WRITABLE_REPO_DIR}/tests

# Run all steps.
${REPO_DIR}/scripts/common_steps/upgrade.sh &&
${REPO_DIR}/scripts/check_format/steps/install_dependencies.sh &&
${REPO_DIR}/scripts/common_steps/create_build_environment.sh ${SOURCE_DIR} ${BUILD_DIR} ON ${CMAKE_C_COMPILER} ${CMAKE_CXX_COMPILER} ${BUILD_TYPE} OFF OFF ON OFF ${UNITS_IMPLEMENTATION} ${VALUES_IMPLEMENTATION} &&
${REPO_DIR}/scripts/common_steps/check_format.sh ${WRITABLE_REPO_DIR} ${BUILD_DIR}
RETURN_CODE=$?

# Make the host user the owner of the build directory.
chown -R 1000:1000 ${BUILD_DIR}

exit ${RETURN_CODE}
