#!/bin/bash

# Check arguments.
if [ $# != 4 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_job.sh REPO_DIR PACKAGE_DIR PACKAGE_NAME DRY_RUN"
  exit 1
fi

REPO_DIR=${1}
PACKAGE_DIR=${2}
PACKAGE_NAME=${3}
DRY_RUN=${4}
SOURCE_DIR=${REPO_DIR}/${PACKAGE_NAME}

# Run all steps.
source ${REPO_DIR}/scripts/common_steps/parse_package_version.sh ${SOURCE_DIR} &&
${REPO_DIR}/scripts/deliver/steps/install_dependencies.sh &&
${REPO_DIR}/scripts/deliver/steps/deliver.sh ${PACKAGE_DIR} ${PACKAGE_NAME} ${PACKAGE_VERSION} ${DRY_RUN}
RETURN_CODE=$?

exit ${RETURN_CODE}
