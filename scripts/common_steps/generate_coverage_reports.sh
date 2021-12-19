#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./generate_coverage_reports.sh SOURCE_DIR BUILD_DIR"
  exit 1
fi

SOURCE_DIR=${1}
BUILD_DIR=${2}

# Generate coverage reports.
cd ${BUILD_DIR}
# Generate HTML and XML reports.
mkdir code_coverage
gcovr --root ${SOURCE_DIR} --exclude ${SOURCE_DIR}/test --html-details -o code_coverage/index.html
gcovr --root ${SOURCE_DIR} --exclude ${SOURCE_DIR}/test --xml-pretty -o code_coverage/output.xml
cd -

exit 0
