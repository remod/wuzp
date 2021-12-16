#!/bin/bash

# Check arguments.
if [ $# != 3 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_job.sh REPO_DIR PACKAGE_DIR PACKAGE_NAME"
  exit 1
fi

REPO_DIR=${1}
PACKAGE_DIR=${2}
PACKAGE_NAME=${3}
SOURCE_DIR=${REPO_DIR}/${PACKAGE_NAME}

# Run all steps.
# TODO(remod): Compare version to version in CMakeLists.
# TODO(remod): When making this repo public, use GitHub environment to restrict deployment secrets to master.
source ${REPO_DIR}/scripts/common_steps/parse_package_version.sh ${SOURCE_DIR} &&
${REPO_DIR}/scripts/package/steps/install_dependencies.sh &&
source ${REPO_DIR}/scripts/package/steps/configure_gpg_key.sh &&
${REPO_DIR}/scripts/package/steps/package.sh ${SOURCE_DIR} ${PACKAGE_DIR} ${PACKAGE_NAME} ${PACKAGE_VERSION} &&
RETURN_CODE=$?

# Make the host user the owner of the package directory.
chown -R 1000:1000 ${PACKAGE_DIR}

exit ${RETURN_CODE}
