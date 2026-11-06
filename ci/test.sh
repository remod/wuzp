#!/usr/bin/env bash
# Configures, builds and tests one preset, e.g. ci/test.sh gcc-wuzp-eigen. Extra arguments are passed to the configure step.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

# WUZP_JOBS limits the parallelism of builds and tests, including the projects built by the compile-fail tests.
export CMAKE_BUILD_PARALLEL_LEVEL=${WUZP_JOBS:-$(nproc)}
export CTEST_PARALLEL_LEVEL=${CMAKE_BUILD_PARALLEL_LEVEL}

PRESET=$1
shift
cmake --preset "${PRESET}" "$@"
cmake --build --preset "${PRESET}"
# WUZP_CTEST_ARGS adds arguments to ctest, e.g. "--label-exclude compile" to skip the compile tests.
# shellcheck disable=SC2086
ctest --preset "${PRESET}" ${WUZP_CTEST_ARGS:-}
