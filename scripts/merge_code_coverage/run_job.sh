#!/bin/bash

# Check arguments.
if [ $# != 3 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./run_job.sh REPO_DIR BUILDS_DIR CODE_COVERAGE_DIR"
  exit 1
fi

REPO_DIR=${1}
BUILDS_DIR=${2}
CODE_COVERAGE_DIR=${3}

# Trust the repository.
git config --global --add safe.directory '*'

# Run all steps.
${REPO_DIR}/scripts/common_steps/upgrade.sh &&
${REPO_DIR}/scripts/merge_code_coverage/steps/install_dependencies.sh &&
${REPO_DIR}/scripts/common_steps/merge_code_coverage.sh ${BUILDS_DIR} ${CODE_COVERAGE_DIR}
RETURN_CODE=$?

# Make the host user the owner of the code coverage directory.
chown -R 1000:1000 ${CODE_COVERAGE_DIR}

exit ${RETURN_CODE}
