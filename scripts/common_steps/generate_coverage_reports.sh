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
# Generate HTML and XML reports.
mkdir code_coverage
gcovr --root ${REPO_DIR} --exclude ${REPO_DIR}/tests --html-details -o code_coverage/index.html
gcovr --root ${REPO_DIR} --exclude ${REPO_DIR}/tests --xml-pretty -o code_coverage/output.xml
cd -

exit 0
