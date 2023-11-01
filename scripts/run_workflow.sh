#!/bin/bash

# This script locally runs the same steps as the Github workflow.
# There are some differences:
# - The workflow merges master into the branch before running the jobs, the script does not.
# - The workflow runs jobs in parallel, the script in sequence.
# - The workflow uploads the code coverage results, the script does not.

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# -lt 3 ] || [ $# -gt 5 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./run_workflow.sh GPG_SIGNING_KEY_FILE GPG_SIGNING_KEYGRIP GPG_SIGNING_KEY_PASSPHRASE [SKIP_CHECK_FORMAT=false] [DOCKER_IMAGE=ubuntu:22.04]"
  exit 1
fi

GPG_SIGNING_KEY_FILE=${1}
export GPG_SIGNING_KEYGRIP=${2}
export GPG_SIGNING_KEY_PASSPHRASE=${3}
if [ $# -ge 4 ]; then
  SKIP_CHECK_FORMAT=${4}
else
  SKIP_CHECK_FORMAT=false
fi
if [ $# -ge 5 ]; then
  DOCKER_IMAGE=${5}
else
  DOCKER_IMAGE="ubuntu:22.04"
  docker pull ${DOCKER_IMAGE}
fi

# Check GPG signing key file.
if [ ! -f ${GPG_SIGNING_KEY_FILE} ]; then
    echo "GPG signing key file '${GPG_SIGNING_KEY_FILE}' does not exist."
    exit 1
fi
GPG_SIGNING_KEY_TMP=$(cat ${GPG_SIGNING_KEY_FILE})
export GPG_SIGNING_KEY=${GPG_SIGNING_KEY_TMP}

# Find out the location of the repository.
SCRIPT_DIR="$( cd "$( dirname ${BASH_SOURCE[0]} )" &> /dev/null && pwd )"
# Define directories.
REPO_DIR=${SCRIPT_DIR}/..
echo "The repository was found in: ${REPO_DIR}"
WORKSPACE_DIR=/root/workspace
TMP_DIR=/tmp/wuzp
CHECK_FORMAT_DIR=${TMP_DIR}/check_format
TESTS_DIR=${TMP_DIR}/test
CODE_COVERAGE_DIR=${TMP_DIR}/code_coverage
PACKAGES_TEST_DIR=${TMP_DIR}/package/test
PACKAGES_SHIP_DIR=${TMP_DIR}/package/ship
TEST_PACKAGES_DIR=${TMP_DIR}/test_packages
# Define other variables.
BUILD_TYPE=Debug
DEBIAN_FRONTEND=noninteractive
export DEBIAN_FRONTEND

print_title () {
  echo "****************************************************************************************"
  echo "* $1"
  echo "****************************************************************************************"
}

# Run workflow.
print_title "Running workflow in docker image ${DOCKER_IMAGE} ..."
mkdir -p ${TMP_DIR}

# Check format.
if [[ ${SKIP_CHECK_FORMAT} = false ]]; then
  print_title "Checking format ..."
  # Instead of modifying the real code with the docker user which then owns the files, modify a copy of it.
  rm -rf ${CHECK_FORMAT_DIR}
  mkdir -p ${CHECK_FORMAT_DIR}
  CHECK_FORMAT_REPO_DIR=${CHECK_FORMAT_DIR}/git
  rm -rf ${CHECK_FORMAT_REPO_DIR}
  cp -r ${REPO_DIR} ${CHECK_FORMAT_REPO_DIR}
  docker run --rm -ti -e DEBIAN_FRONTEND -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${CHECK_FORMAT_DIR}:${CHECK_FORMAT_DIR}:rw ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/check_format/run_job.sh ${WORKSPACE_DIR} ${CHECK_FORMAT_REPO_DIR} ${CHECK_FORMAT_DIR}/check_format
fi

# Check package versions.
print_title "Checking package versions ..."
for PACKAGE_NAME in "wuzp" "wuzp_blitz" "wuzp_boost" "wuzp_eigen"; do
  docker run --rm -ti -e DEBIAN_FRONTEND -v ${REPO_DIR}:${WORKSPACE_DIR}:ro ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/check_package_version/run_job.sh ${WORKSPACE_DIR} ${PACKAGE_NAME}
done

# Test.
for CMAKE_C_CXX_COMPILER in "clang_clang++" "gcc_g++"; do
  for UNITS_IMPLEMENTATION in 0 1; do
    for VALUES_IMPLEMENTATION in 1 2; do
      print_title "Testing with ${CMAKE_C_CXX_COMPILER//_/ } ${UNITS_IMPLEMENTATION} ${VALUES_IMPLEMENTATION} ..."
      TEST_DIR=${TESTS_DIR}/${CMAKE_C_CXX_COMPILER}_${UNITS_IMPLEMENTATION}_${VALUES_IMPLEMENTATION}
      rm -rf ${TEST_DIR}
      mkdir -p ${TEST_DIR}
      if [[ ${CMAKE_C_CXX_COMPILER} == "clang_clang++" ]]; then
        ENABLE_CODE_COVERAGE=true
      else
        ENABLE_CODE_COVERAGE=false
      fi
      docker run --rm -ti -e DEBIAN_FRONTEND -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${TEST_DIR}:${TEST_DIR}:rw ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/test/run_job.sh ${WORKSPACE_DIR} ${TEST_DIR} ${ENABLE_CODE_COVERAGE} ${CMAKE_C_CXX_COMPILER//_/ } ${BUILD_TYPE} ${UNITS_IMPLEMENTATION} ${VALUES_IMPLEMENTATION}
    done
  done
done

# Merge code coverage.
print_title "Merging code coverage ..."
rm -rf ${CODE_COVERAGE_DIR}
mkdir -p ${CODE_COVERAGE_DIR}
docker run --rm -ti -e DEBIAN_FRONTEND -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${TESTS_DIR}:${TESTS_DIR}:ro -v ${CODE_COVERAGE_DIR}:${CODE_COVERAGE_DIR}:rw ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/merge_code_coverage/run_job.sh ${WORKSPACE_DIR} ${TESTS_DIR} ${CODE_COVERAGE_DIR}

# Package wuzp.
print_title "Packaging wuzp ..."
PACKAGE_TEST_DIR=${PACKAGES_TEST_DIR}/wuzp
rm -rf ${PACKAGE_TEST_DIR}
mkdir -p ${PACKAGE_TEST_DIR}
PACKAGE_SHIP_DIR=${PACKAGES_SHIP_DIR}/wuzp
rm -rf ${PACKAGE_SHIP_DIR}
mkdir -p ${PACKAGE_SHIP_DIR}
docker run --rm -ti -e DEBIAN_FRONTEND -e GPG_SIGNING_KEY -e GPG_SIGNING_KEYGRIP -e GPG_SIGNING_KEY_PASSPHRASE -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${PACKAGE_TEST_DIR}:${PACKAGE_TEST_DIR}:rw -v ${PACKAGE_SHIP_DIR}:${PACKAGE_SHIP_DIR}:rw ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/package/run_job.sh ${WORKSPACE_DIR} ${PACKAGE_TEST_DIR} ${PACKAGE_SHIP_DIR} wuzp

# Package extensions.
WUZP_PACKAGE_TEST_DIR=${PACKAGE_TEST_DIR}
for PACKAGE_NAME in "wuzp_boost" "wuzp_eigen"; do
  print_title "Packaging ${PACKAGE_NAME} ..."
  PACKAGE_TEST_DIR=${PACKAGES_TEST_DIR}/${PACKAGE_NAME}
  rm -rf ${PACKAGE_TEST_DIR}
  mkdir -p ${PACKAGE_TEST_DIR}
  PACKAGE_SHIP_DIR=${PACKAGES_SHIP_DIR}/${PACKAGE_NAME}
  rm -rf ${PACKAGE_SHIP_DIR}
  mkdir -p ${PACKAGE_SHIP_DIR}
  docker run --rm -ti -e DEBIAN_FRONTEND -e GPG_SIGNING_KEY -e GPG_SIGNING_KEYGRIP -e GPG_SIGNING_KEY_PASSPHRASE -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${WUZP_PACKAGE_TEST_DIR}:${WUZP_PACKAGE_TEST_DIR}:ro -v ${PACKAGE_TEST_DIR}:${PACKAGE_TEST_DIR}:rw -v ${PACKAGE_SHIP_DIR}:${PACKAGE_SHIP_DIR}:rw ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/package/run_job.sh ${WORKSPACE_DIR} ${PACKAGE_TEST_DIR} ${PACKAGE_SHIP_DIR} ${PACKAGE_NAME} ${WUZP_PACKAGE_TEST_DIR}
done

# Test packages.
for CMAKE_C_CXX_COMPILER in "clang_clang++" "gcc_g++"; do
  for UNITS_IMPLEMENTATION in 0 1; do
    for VALUES_IMPLEMENTATION in 1 2; do
      print_title "Testing packages with ${CMAKE_C_CXX_COMPILER//_/ } ${UNITS_IMPLEMENTATION} ${VALUES_IMPLEMENTATION} ..."
      TEST_PACKAGE_DIR=${TEST_PACKAGES_DIR}/${CMAKE_C_CXX_COMPILER}_${UNITS_IMPLEMENTATION}_${VALUES_IMPLEMENTATION}
      rm -rf ${TEST_PACKAGE_DIR}
      mkdir -p ${TEST_PACKAGE_DIR}
      docker run --rm -ti -e DEBIAN_FRONTEND -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${PACKAGES_TEST_DIR}:${PACKAGES_TEST_DIR}:ro -v ${TEST_PACKAGE_DIR}:${TEST_PACKAGE_DIR}:rw ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/test_packages/run_job.sh ${WORKSPACE_DIR} ${PACKAGES_TEST_DIR} ${TEST_PACKAGE_DIR} ${CMAKE_C_CXX_COMPILER//_/ } ${BUILD_TYPE} ${UNITS_IMPLEMENTATION} ${VALUES_IMPLEMENTATION}
    done
  done
done

# Deliver.
for PACKAGE_NAME in "wuzp" "wuzp_boost" "wuzp_eigen"; do
  print_title "Delivering package ${PACKAGE_NAME} ..."
  PACKAGE_DIR=${PACKAGES_SHIP_DIR}/${PACKAGE_NAME}
  docker run --rm -ti -e DEBIAN_FRONTEND -e GPG_SIGNING_KEY -v ${REPO_DIR}:${WORKSPACE_DIR}:ro -v ${PACKAGE_DIR}:${PACKAGE_DIR}:ro ${DOCKER_IMAGE} /bin/bash ${WORKSPACE_DIR}/scripts/deliver/run_job.sh ${WORKSPACE_DIR} ${PACKAGE_DIR} ${PACKAGE_NAME} true
done

print_title "Done."
exit 0
