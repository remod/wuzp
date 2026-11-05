#!/usr/bin/env bash
# Installs wuzp with all backends, then builds the tests and examples against the installed package, like a user would.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

# WUZP_JOBS limits the parallelism of builds and tests, including the projects built by the compile-fail tests.
export CMAKE_BUILD_PARALLEL_LEVEL=${WUZP_JOBS:-$(nproc)}
export CTEST_PARALLEL_LEVEL=${CMAKE_BUILD_PARALLEL_LEVEL}

PREFIX=${PWD}/build/prefix
cmake -S . -B build/install -G Ninja -D WUZP_WITH_EIGEN=ON -D WUZP_WITH_BLITZ=ON -D WUZP_WITH_BOOST=ON \
  -D WUZP_BUILD_TESTS=OFF -D WUZP_BUILD_EXAMPLES=OFF -D CMAKE_INSTALL_PREFIX="${PREFIX}"
cmake --install build/install

cmake -S tests -B build/consume-tests -G Ninja -D CMAKE_PREFIX_PATH="${PREFIX}" -D WUZP_TEST_UNITS=boost -D WUZP_TEST_VALUES=eigen
cmake --build build/consume-tests
ctest --test-dir build/consume-tests --output-on-failure --no-tests=error

cmake -S examples -B build/consume-examples -G Ninja -D CMAKE_PREFIX_PATH="${PREFIX}"
cmake --build build/consume-examples
