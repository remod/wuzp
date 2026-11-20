#!/usr/bin/env bash
# Configures, builds and tests one preset, e.g. ci/test.sh gcc-wuzp-eigen. Extra arguments are passed to the configure step.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

# WUZP_JOBS limits the parallelism of builds and tests, including the projects built by the compile-fail tests.
export CMAKE_BUILD_PARALLEL_LEVEL=${WUZP_JOBS:-$(getconf _NPROCESSORS_ONLN)}
export CTEST_PARALLEL_LEVEL=${CMAKE_BUILD_PARALLEL_LEVEL}

PRESET=$1
shift
cmake --preset "${PRESET}" "$@"
# A failed object does not stop the build (Ninja's -k 0, every preset uses Ninja), so that one run shows every compile error,
# e.g. those of MSVC, which only runs in CI. The build still fails.
cmake --build --preset "${PRESET}" -- -k 0
# WUZP_CTEST_ARGS adds arguments to ctest, e.g. "--label-exclude compile" to skip the compile tests.
# shellcheck disable=SC2086
ctest --preset "${PRESET}" ${WUZP_CTEST_ARGS:-}
