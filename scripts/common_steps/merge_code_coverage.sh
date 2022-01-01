#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./merge_code_coverage.sh CODE_COVERAGE_REPORTS CODE_COVERAGE_DIR"
  exit 1
fi

CODE_COVERAGE_REPORTS=${1}
CODE_COVERAGE_DIR=${2}

# Generate HTML report for local inspection. If less than one line is covered, it is likely a configuration error.
gcovr --add-tracefile "${CODE_COVERAGE_REPORTS}" --fail-under-line 1 --verbose --html-details --output ${CODE_COVERAGE_DIR}/index.html
# Generate XML report for uploading to codecov. If less than one line is covered, it is likely a configuration error.
gcovr --add-tracefile "${CODE_COVERAGE_REPORTS}" --fail-under-line 1 --verbose --xml-pretty --output ${CODE_COVERAGE_DIR}/output.xml

exit 0
