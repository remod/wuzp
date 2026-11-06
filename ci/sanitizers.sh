#!/usr/bin/env bash
# Runs the tests with AddressSanitizer and UndefinedBehaviorSanitizer, for every backend combination.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

for PRESET in clang-wuzp-eigen clang-wuzp-blitz clang-boost-eigen clang-boost-blitz; do
  ci/test.sh "${PRESET}" -D WUZP_ENABLE_SANITIZERS=ON
done
