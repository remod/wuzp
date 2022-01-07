#!/bin/bash

# Check arguments.
if [ $# != 3 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./run_job.sh REPO_DIR WRITABLE_REPO_DIR PACKAGE_NAME"
  exit 1
fi

REPO_DIR=${1}
WRITABLE_REPO_DIR=${2}
PACKAGE_NAME=${3}
SOURCE_DIR=${WRITABLE_REPO_DIR}/${PACKAGE_NAME}

# Run all steps.
${REPO_DIR}/scripts/common_steps/upgrade.sh &&
${REPO_DIR}/scripts/check_package_version/steps/install_dependencies.sh &&
source ${REPO_DIR}/scripts/common_steps/folder_changed.sh ${WRITABLE_REPO_DIR} ${SOURCE_DIR}
if [ ${FOLDER_CHANGED} -eq 1 ]; then
  source ${REPO_DIR}/scripts/common_steps/parse_package_version.sh ${SOURCE_DIR} &&
  PACKAGE_VERSION_BRANCH=${PACKAGE_VERSION} &&
  export PACKAGE_VERSION_BRANCH &&
  git -C ${WRITABLE_REPO_DIR} reset --hard &&
  git -C ${WRITABLE_REPO_DIR} checkout master &&
  source ${REPO_DIR}/scripts/common_steps/parse_package_version.sh ${SOURCE_DIR} &&
  PACKAGE_VERSION_MASTER=${PACKAGE_VERSION} &&
  export PACKAGE_VERSION_MASTER &&
  ${REPO_DIR}/scripts/common_steps/check_package_version.sh
fi

exit $?
