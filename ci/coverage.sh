#!/usr/bin/env bash
# Measures the code coverage of the tests, merged across all backend combinations, and writes build/coverage.lcov.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

# The compile tests add nothing to the runtime coverage, and the test jobs run them already.
export WUZP_CTEST_ARGS="--label-exclude compile"

PRESETS=(clang-wuzp-eigen clang-wuzp-blitz clang-boost-eigen clang-boost-blitz)
IGNORE='(tests|/usr)/'

rm -rf build/profiles
OBJECTS=()
for PRESET in "${PRESETS[@]}"; do
  LLVM_PROFILE_FILE="${PWD}/build/profiles/${PRESET}-%p.profraw" ci/test.sh "${PRESET}" -D WUZP_ENABLE_COVERAGE=ON
  OBJECTS+=(--object "build/${PRESET}/tests/wuzp_tests")
done

llvm-profdata merge --sparse build/profiles/*.profraw -o build/coverage.profdata
llvm-cov report --instr-profile build/coverage.profdata --ignore-filename-regex "${IGNORE}" "${OBJECTS[@]}"
llvm-cov export --format lcov --instr-profile build/coverage.profdata --ignore-filename-regex "${IGNORE}" "${OBJECTS[@]}" \
  > build/coverage.lcov
