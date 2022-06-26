#!/bin/bash

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./run_job.sh REPO_DIR PACKAGE_NAME"
  exit 1
fi

REPO_DIR=${1}
PACKAGE_NAME=${2}
WRITABLE_REPO_DIR=/tmp/wuzp/check_package_versions
SOURCE_DIR=${REPO_DIR}/${PACKAGE_NAME}
WRITABLE_SOURCE_DIR=${WRITABLE_REPO_DIR}/${PACKAGE_NAME}

# Trust the repository.
git config --global --add safe.directory '*'

# Run all steps.
${REPO_DIR}/scripts/common_steps/upgrade.sh &&
${REPO_DIR}/scripts/check_package_version/steps/install_dependencies.sh &&
source ${REPO_DIR}/scripts/common_steps/folder_changed.sh ${REPO_DIR} ${SOURCE_DIR}
if [ ${FOLDER_CHANGED} -eq 1 ]; then
  ${REPO_DIR}/scripts/common_steps/copy_folder.sh ${REPO_DIR} ${WRITABLE_REPO_DIR} &&
  source ${REPO_DIR}/scripts/common_steps/parse_package_version.sh ${WRITABLE_SOURCE_DIR} &&
  PACKAGE_VERSION_BRANCH=${PACKAGE_VERSION} &&
  ${REPO_DIR}/scripts/common_steps/checkout_branch.sh ${WRITABLE_REPO_DIR} master &&
  source ${REPO_DIR}/scripts/common_steps/parse_package_version.sh ${WRITABLE_SOURCE_DIR} &&
  PACKAGE_VERSION_MASTER=${PACKAGE_VERSION} &&
  ${REPO_DIR}/scripts/common_steps/check_package_version.sh ${PACKAGE_VERSION_BRANCH} ${PACKAGE_VERSION_MASTER}
fi

exit $?
