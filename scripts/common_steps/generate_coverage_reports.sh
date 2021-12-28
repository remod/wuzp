#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./generate_coverage_reports.sh REPO_DIR BUILD_DIR"
  exit 1
fi

REPO_DIR=${1}
BUILD_DIR=${2}

# Generate coverage reports.
cd ${BUILD_DIR}
mkdir code_coverage
# Generate HTML report for local inspection. If less than one line is covered, it is likely a configuration error.
gcovr --root ${REPO_DIR} --fail-under-line 1 --verbose --html-details --output code_coverage/index.html ${BUILD_DIR}
# Generate XML report for uploading to codecov. If less than one line is covered, it is likely a configuration error.
gcovr --root ${REPO_DIR} --fail-under-line 1 --verbose --xml-pretty --output code_coverage/output.xml ${BUILD_DIR}
cd -
# Check if coverage is valid.

exit 0
