#!/bin/bash

# Check arguments.
if [ $# != 8 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./run_job.sh REPO_DIR PACKAGES_DIR BUILD_DIR CMAKE_C_COMPILER CMAKE_CXX_COMPILER BUILD_TYPE UNITS_IMPLEMENTATION VALUES_IMPLEMENTATION"
  exit 1
fi

REPO_DIR=${1}
PACKAGES_DIR=${2}
BUILD_DIR=${3}
CMAKE_C_COMPILER=${4}
CMAKE_CXX_COMPILER=${5}
BUILD_TYPE=${6}
UNITS_IMPLEMENTATION=${7}
VALUES_IMPLEMENTATION=${8}
SOURCE_DIR=${REPO_DIR}/tests

# Trust the repository.
git config --global --add safe.directory '*'

# Run all steps.
${REPO_DIR}/scripts/common_steps/upgrade.sh &&
${REPO_DIR}/scripts/test_packages/steps/install_dependencies.sh ${UNITS_IMPLEMENTATION} ${VALUES_IMPLEMENTATION} &&
${REPO_DIR}/scripts/test_packages/steps/install_packages.sh ${PACKAGES_DIR} ${UNITS_IMPLEMENTATION} ${VALUES_IMPLEMENTATION} &&
${REPO_DIR}/scripts/common_steps/create_build_environment.sh ${SOURCE_DIR} ${BUILD_DIR} OFF ${CMAKE_C_COMPILER} ${CMAKE_CXX_COMPILER} ${BUILD_TYPE} ON OFF OFF OFF ${UNITS_IMPLEMENTATION} ${VALUES_IMPLEMENTATION} &&
${REPO_DIR}/scripts/common_steps/build.sh ${BUILD_DIR} ${BUILD_TYPE} &&
${REPO_DIR}/scripts/common_steps/test.sh ${BUILD_DIR} ${BUILD_TYPE}
RETURN_CODE=$?

# Make the host user the owner of the build directory.
chown -R 1000:1000 ${BUILD_DIR}

exit ${RETURN_CODE}
