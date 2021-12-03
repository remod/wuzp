#!/bin/bash

# This script locally runs the same steps as the Github workflow.
# The only difference is that the script performs an out-of-source build in the mounted /tmp directory.

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# -gt 1 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_workflow.sh [docker-image=ubuntu:latest]"
  exit 1
fi

if [ $# == 1 ]; then
  DOCKER_IMAGE=${1}
else
  DOCKER_IMAGE=ubuntu:latest
fi

# Find out the location of the repository.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_DIR=${SCRIPT_DIR}/..

# Run all jobs of the Github workflow.
WORKSPACE_DIR=/root
BUILD_TYPE=Debug

# Build and test.
#for C_CXX_COMPILER in "gcc g++" "clang clang++"; do
#  for UNITS_IMPLEMENTATION in 0 1; do
#    for VALUE_MATRIX_IMPLEMENTATION in 1; do
#      # Run job.
#      echo "Building and testing ${C_CXX_COMPILER} ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} ..."
#      BUILD_DIR=/tmp/build_${C_CXX_COMPILER}_${UNITS_IMPLEMENTATION}_${VALUE_MATRIX_IMPLEMENTATION}
#      rm -rf ${BUILD_DIR}
#      mkdir ${BUILD_DIR}
#      docker run --rm -ti -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${BUILD_DIR}:${BUILD_DIR}:rw ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/build_and_test/run_job.sh ${WORKSPACE_DIR} ${BUILD_DIR} ${PACKAGE_DIR} ${C_CXX_COMPILER} ${BUILD_TYPE} ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION}
#    done
#  done
#done

# Deliver.
for TARGET_NAME in "wuzp"; do
  PACKAGE_DIR=/tmp/package_${TARGET_NAME}
  rm -rf ${PACKAGE_DIR}
  mkdir ${PACKAGE_DIR}
  GNUPG_DIR=/tmp/.gnupg
  rm -rf ${GNUPG_DIR}
  mkdir -p ${GNUPG_DIR}
  docker run --rm -ti -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${HOME}/.gnupg:/tmp/.gnupg:rw -v ${PACKAGE_DIR}:${PACKAGE_DIR}:rw ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/deliver/run_job.sh ${WORKSPACE_DIR} ${PACKAGE_DIR} ${TARGET_NAME} ${GNUPG_DIR} true
done

echo "Done."
exit 0
