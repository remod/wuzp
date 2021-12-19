#!/bin/bash

# This script locally runs the same steps as the Github workflow.
# The only difference is that the script performs an out-of-source build in the mounted /tmp directory.

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# -lt 3 ] || [ $# -gt 4 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_workflow.sh GPG_SIGNING_KEY_FILE GPG_SIGNING_KEYGRIP GPG_SIGNING_KEY_PASSPHRASE [DOCKER_IMAGE=ubuntu:20.04]"
  exit 1
fi

GPG_SIGNING_KEY_FILE=${1}
export GPG_SIGNING_KEYGRIP=${2}
export GPG_SIGNING_KEY_PASSPHRASE=${3}
if [ $# == 4 ]; then
  DOCKER_IMAGE=${4}
else
  DOCKER_IMAGE="ubuntu:20.04"
fi

# Check GPG signing key file.
if [ ! -f ${GPG_SIGNING_KEY_FILE} ]; then
    echo "GPG signing key file '${GPG_SIGNING_KEY_FILE}' does not exist."
    exit 1
fi

# Find out the location of the repository.
SCRIPT_DIR="$( cd "$( dirname ${BASH_SOURCE[0]} )" &> /dev/null && pwd )"
REPO_DIR=${SCRIPT_DIR}/..

# Run all jobs of the Github workflow.
WORKSPACE_DIR=/root
BUILD_TYPE=Debug

# Run workflow.
echo "Running workflow in docker image ${DOCKER_IMAGE} ..."

# Check format.
echo "Checking format ..."
BUILD_DIR=/tmp/wuzp/check_format
rm -rf ${BUILD_DIR}
mkdir -p ${BUILD_DIR}
docker run --rm -ti -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${BUILD_DIR}:${BUILD_DIR}:rw ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/check_format/run_job.sh ${WORKSPACE_DIR} ${BUILD_DIR}

# Test.
for C_CXX_COMPILER in "gcc g++" "clang clang++"; do
  for UNITS_IMPLEMENTATION in 0 1; do
    for VALUE_MATRIX_IMPLEMENTATION in 1; do
      # Run job.
      echo "Testing with ${C_CXX_COMPILER} ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} ..."
      BUILD_DIR=/tmp/wuzp/test/${C_CXX_COMPILER// /_}_${UNITS_IMPLEMENTATION}_${VALUE_MATRIX_IMPLEMENTATION}
      rm -rf ${BUILD_DIR}
      mkdir -p ${BUILD_DIR}
      docker run --rm -ti -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${BUILD_DIR}:${BUILD_DIR}:rw ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/test/run_job.sh ${WORKSPACE_DIR} ${BUILD_DIR} ${C_CXX_COMPILER} ${BUILD_TYPE} ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION}
    done
  done
done

# Package.
GPG_SIGNING_KEY_TMP=$(cat ${GPG_SIGNING_KEY_FILE})
export GPG_SIGNING_KEY=${GPG_SIGNING_KEY_TMP}
for PACKAGE_NAME in "wuzp" "wuzp_boost" "wuzp_eigen"; do
  # Run job.
  echo "Packaging ${PACKAGE_NAME} ..."
  PACKAGE_DIR=/tmp/wuzp/package/${PACKAGE_NAME}
  rm -rf ${PACKAGE_DIR}
  mkdir -p ${PACKAGE_DIR}
  docker run --rm -ti -e GPG_SIGNING_KEY -e GPG_SIGNING_KEYGRIP -e GPG_SIGNING_KEY_PASSPHRASE -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${PACKAGE_DIR}:${PACKAGE_DIR}:rw ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/package/run_job.sh ${WORKSPACE_DIR} ${PACKAGE_DIR} ${PACKAGE_NAME}
done

# Test packages.
for C_CXX_COMPILER in "gcc g++" "clang clang++"; do
  for UNITS_IMPLEMENTATION in 0 1; do
    for VALUE_MATRIX_IMPLEMENTATION in 1; do
      # Run job.
      echo "Testing packages with ${C_CXX_COMPILER} ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION} ..."
      PACKAGES_DIR=/tmp/wuzp/package
      BUILD_DIR=/tmp/wuzp/test_packages/${C_CXX_COMPILER// /_}_${UNITS_IMPLEMENTATION}_${VALUE_MATRIX_IMPLEMENTATION}
      rm -rf ${BUILD_DIR}
      mkdir -p ${BUILD_DIR}
      docker run --rm -ti -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${PACKAGES_DIR}:${PACKAGES_DIR}:ro -v ${BUILD_DIR}:${BUILD_DIR}:rw ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/test_packages/run_job.sh ${WORKSPACE_DIR} ${PACKAGE_DIR} ${BUILD_DIR} ${C_CXX_COMPILER} ${BUILD_TYPE} ${UNITS_IMPLEMENTATION} ${VALUE_MATRIX_IMPLEMENTATION}
    done
  done
done

# Deliver.
for PACKAGE_NAME in "wuzp" "wuzp_boost" "wuzp_eigen"; do
  # Run job.
  echo "Delivering package ${PACKAGE_NAME} ..."
  PACKAGE_DIR=/tmp/wuzp/package/${PACKAGE_NAME}
  docker run --rm -ti -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${PACKAGE_DIR}:${PACKAGE_DIR}:ro ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/deliver/run_job.sh ${WORKSPACE_DIR} ${PACKAGE_DIR} ${PACKAGE_NAME} true
done

echo "Done."
exit 0
