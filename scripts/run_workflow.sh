#!/bin/bash

# This script locally runs the same steps as the Github workflow.
# The only difference is that the script performs an out-of-source build in the mounted /tmp directory.

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 0 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_workflow.sh"
  exit 1
fi

# Find out the location of the repository.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_DIR=${SCRIPT_DIR}/..

# Run all jobs of the Github workflow.
WORKSPACE_DIR=/root
BUILD_TYPE=Release
for C_CXX_COMPILER in "gcc g++" "clang clang++"; do
  for UNITS_IMPLEMENTATION in 0 1; do
    for MATRIX_IMPLEMENTATION in 1; do
      # Run job.
      echo "Building and testing ${C_CXX_COMPILER} ${UNITS_IMPLEMENTATION} ${MATRIX_IMPLEMENTATION} ..."
      BUILD_DIR=/tmp/build_${C_CXX_COMPILER}_${UNITS_IMPLEMENTATION}_${MATRIX_IMPLEMENTATION}
      rm -rf ${BUILD_DIR}
      mkdir ${BUILD_DIR}
      docker run --rm -ti -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${BUILD_DIR}:${BUILD_DIR}:rw ubuntu:latest /bin/bash ${WORKSPACE_DIR}/scripts/run_job.sh ${WORKSPACE_DIR} ${BUILD_DIR} ${C_CXX_COMPILER} ${BUILD_TYPE} ${UNITS_IMPLEMENTATION} ${MATRIX_IMPLEMENTATION}
    done
  done
done
echo "Success."
