#!/bin/bash

# Check arguments.
if [ $# != 5 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_job.sh WORKSPACE_DIR BUILD_DIR BUILD_TYPE UNITS_IMPLEMENTATION MATRIX_IMPLEMENTATION"
  exit 1
fi

WORKSPACE_DIR=${1}
BUILD_DIR=${2}
BUILD_TYPE=${3}
UNITS_IMPLEMENTATION=${4}
MATRIX_IMPLEMENTATION=${5}

# Run all steps.
${WORKSPACE_DIR}/scripts/steps/install_dependencies.sh ${UNITS_IMPLEMENTATION} ${MATRIX_IMPLEMENTATION} &&
${WORKSPACE_DIR}/scripts/steps/create_build_environment.sh ${WORKSPACE_DIR} ${BUILD_DIR} ${BUILD_TYPE} ON ON ON ${UNITS_IMPLEMENTATION} ${MATRIX_IMPLEMENTATION} &&
${WORKSPACE_DIR}/scripts/steps/build.sh ${BUILD_DIR} ${BUILD_TYPE} &&
${WORKSPACE_DIR}/scripts/steps/test.sh ${BUILD_DIR} ${BUILD_TYPE} &&
${WORKSPACE_DIR}/scripts/steps/format.sh ${BUILD_DIR} &&
${WORKSPACE_DIR}/scripts/steps/tidy.sh ${BUILD_DIR}
RETURN_CODE=$?

# Make the host user the owner of the build directory.
chown -R 1000:1000 ${BUILD_DIR}

exit ${RETURN_CODE}
