#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./merge_code_coverage.sh BUILDS_DIR CODE_COVERAGE_DIR"
  exit 1
fi

BUILDS_DIR=${1}
CODE_COVERAGE_DIR=${2}
mkdir -p ${CODE_COVERAGE_DIR}

# Merge code coverage reports.
CODE_COVERAGE_REPORTS="${BUILDS_DIR}/*/default.profraw"
llvm-profdata merge --sparse --output ${CODE_COVERAGE_DIR}/default.profdata ${CODE_COVERAGE_REPORTS}

# Generate HTML report.
BUILD_DIRS="$BUILDS_DIR/*"
OBJECTS=""
for BUILD_DIR in $BUILD_DIRS; do
  OBJECTS="$OBJECTS --object $BUILD_DIR/tests"
done
llvm-cov show --ignore-filename-regex tests --show-instantiations=false --output-dir ${CODE_COVERAGE_DIR}/html --format "html" --instr-profile ${CODE_COVERAGE_DIR}/default.profdata ${OBJECTS}
llvm-cov show --ignore-filename-regex tests --show-instantiations=false --instr-profile ${CODE_COVERAGE_DIR}/default.profdata ${OBJECTS} > ${CODE_COVERAGE_DIR}/coverage.txt

exit 0
