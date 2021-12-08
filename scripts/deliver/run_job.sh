#!/bin/bash

# Check arguments.
if [ $# != 4 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_job.sh REPO_DIR PACKAGE_DIR TARGET_NAME DRY_RUN"
  exit 1
fi

REPO_DIR=${1}
PACKAGE_DIR=${2}
TARGET_NAME=${3}
DRY_RUN=${4}
SOURCE_DIR=${REPO_DIR}/${TARGET_NAME}

# Run all steps.
# TODO(remod): Compare version to version in CMakeLists.
source "${REPO_DIR}"/scripts/deliver/steps/parse_package_version.sh "${SOURCE_DIR}" &&
${REPO_DIR}/scripts/deliver/steps/install_dependencies.sh &&
source "${REPO_DIR}"/scripts/deliver/steps/configure_gpg_key.sh &&
${REPO_DIR}/scripts/deliver/steps/package.sh "${SOURCE_DIR}" ${PACKAGE_DIR} ${TARGET_NAME} ${TARGET_VERSION} ${GNUPG_DIR} &&
${REPO_DIR}/scripts/deliver/steps/deliver.sh "${PACKAGE_DIR}" ${TARGET_NAME} ${TARGET_VERSION} ${BUILD_INCREMENT} ${DRY_RUN}
RETURN_CODE=$?

# Make the host user the owner of the package directory.
chown -R 1000:1000 "${PACKAGE_DIR}"

exit ${RETURN_CODE}
