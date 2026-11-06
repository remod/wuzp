#!/usr/bin/env bash
# Runs clang-tidy (see .clang-tidy) for every backend combination, or for the given presets, e.g.
# ci/tidy.sh clang-wuzp-eigen. Findings are errors.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

PRESETS=("$@")
if [[ ${#PRESETS[@]} -eq 0 ]]; then
  PRESETS=(clang-wuzp-eigen clang-wuzp-blitz clang-boost-eigen clang-boost-blitz)
fi

# Like the test jobs, a failing preset does not stop the others.
FAILED=()
for PRESET in "${PRESETS[@]}"; do
  echo "=== ${PRESET}"
  cmake --preset "${PRESET}" > /dev/null
  # Checks the translation units of the tests, the examples and the header check, which compiles every public header on
  # its own, and reports findings in wuzp's own files. The compile test cases are left out: most fail on purpose.
  run-clang-tidy -p "build/${PRESET}" -j "${WUZP_JOBS:-$(nproc)}" -quiet -warnings-as-errors='*' \
    -header-filter="^${PWD}/(wuzp|wuzp_eigen|wuzp_blitz|wuzp_boost|tests|examples)/" \
    "^${PWD}/(tests/src|examples|build/${PRESET}/tests/header_check)/" || FAILED+=("${PRESET}")
done
if [[ ${#FAILED[@]} -gt 0 ]]; then
  echo "clang-tidy found problems with: ${FAILED[*]}" >&2
  exit 1
fi
