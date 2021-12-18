#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 3 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./test.sh SOURCE_DIR BUILD_DIR BUILD_CONFIG"
  exit 1
fi

# Test.
# Unfortunately ctest < 3.20 does not support specifying the test directory, so we cd there.
cd ${2}
# If some tests fail, rerun them in verbose mode.
ctest -C ${3} --rerun-failed --output-on-failure --parallel
# Generate HTML and XML reports.
mkdir code_coverage
gcovr --root ${1} --exclude ${1}/test --html-details -o code_coverage/index.html
gcovr --root ${1} --exclude ${1}/test --xml-pretty -o code_coverage/output.xml
cd -

exit 0
