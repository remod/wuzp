#!/bin/bash

# Check arguments.
if [ $# != 3 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./run_job.sh REPO_DIR CODE_COVERAGE_REPORTS CODE_COVERAGE_DIR"
  exit 1
fi

REPO_DIR=${1}
CODE_COVERAGE_REPORTS=${2}
CODE_COVERAGE_DIR=${3}

# Run all steps.
${REPO_DIR}/scripts/common_steps/upgrade.sh &&
${REPO_DIR}/scripts/test/steps/install_dependencies.sh &&
${REPO_DIR}/scripts/common_steps/merge_code_coverage.sh ${CODE_COVERAGE_REPORTS} ${CODE_COVERAGE_DIR}
RETURN_CODE=$?

# Make the host user the owner of the build directory.
chown -R 1000:1000 ${BUILD_DIR}

exit ${RETURN_CODE}
