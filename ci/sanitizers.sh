#!/usr/bin/env bash
# Runs the tests with AddressSanitizer and UndefinedBehaviorSanitizer, for every backend combination.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

# The compile tests have no runtime behavior to check, and the test jobs run them already.
export WUZP_CTEST_ARGS="--label-exclude compile"

for PRESET in clang-wuzp-eigen clang-wuzp-blitz clang-boost-eigen clang-boost-blitz; do
  ci/test.sh "${PRESET}" -D WUZP_ENABLE_SANITIZERS=ON
done
