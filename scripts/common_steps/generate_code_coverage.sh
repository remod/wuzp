#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 4 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./generate_code_coverage.sh ENABLE_CODE_COVERAGE REPO_DIR BUILD_DIR CODE_COVERAGE_DIR"
  exit 1
fi

ENABLE_CODE_COVERAGE=${1}
REPO_DIR=${2}
BUILD_DIR=${3}
CODE_COVERAGE_DIR=${4}

# Return early if code coverage is not enabled.
if [[ ${ENABLE_CODE_COVERAGE} = false ]]; then
  exit 0;
fi

# Generate JSON report for this test configuration. If less than one line is covered, it is likely a configuration error.
#gcovr --root ${REPO_DIR} --exclude ${REPO_DIR}/tests --verbose --json --output ${CODE_COVERAGE_DIR}/report.json ${BUILD_DIR}

exit 0
