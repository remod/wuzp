#!/bin/bash

# Exit immediately if a command fails.
set -E

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_workflow.sh UNITS_IMPLEMENTATION MATRIX_IMPLEMENTATION"
  exit 1
fi
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_DIR=${SCRIPT_DIR}/..

# Run all steps of the Github workflow.
WORKSPACE_DIR=/root
BUILD_DIR=${WORKSPACE_DIR}/build
BUILD_TYPE=Release
UNITS_IMPLEMENTATION=${1}
MATRIX_IMPLEMENTATION=${2}
docker run --rm -ti -v ${REPO_DIR}/:${WORKSPACE_DIR}:rw ubuntu_w_deps /bin/bash -c "\
    source ${WORKSPACE_DIR}/scripts/export_environment_constants.sh && \
    ${WORKSPACE_DIR}/scripts/install_dependencies.sh ${UNITS_IMPLEMENTATION} ${MATRIX_IMPLEMENTATION} && \
    ${WORKSPACE_DIR}/scripts/create_build_environment.sh ${WORKSPACE_DIR} ${BUILD_DIR} ${BUILD_TYPE} ON ON ON ${UNITS_IMPLEMENTATION} ${MATRIX_IMPLEMENTATION} && \
    ${WORKSPACE_DIR}/scripts/build.sh ${BUILD_DIR} ${BUILD_TYPE} && \
    ${WORKSPACE_DIR}/scripts/test.sh ${BUILD_DIR} ${BUILD_TYPE} && \
    ${WORKSPACE_DIR}/scripts/format.sh ${BUILD_DIR} && \
    ${WORKSPACE_DIR}/scripts/tidy.sh ${BUILD_DIR}"
