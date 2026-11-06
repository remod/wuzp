#!/usr/bin/env bash
# Installs wuzp with all backends, then builds the tests and examples against the installed package, and uses wuzp through
# FetchContent, like a user would.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

# WUZP_JOBS limits the parallelism of builds and tests, including the projects built by the compile-fail tests.
export CMAKE_BUILD_PARALLEL_LEVEL=${WUZP_JOBS:-$(getconf _NPROCESSORS_ONLN)}
export CTEST_PARALLEL_LEVEL=${CMAKE_BUILD_PARALLEL_LEVEL}

PREFIX=${PWD}/build/prefix
cmake -S . -B build/install -G Ninja -D WUZP_WITH_EIGEN=ON -D WUZP_WITH_BLITZ=ON -D WUZP_WITH_BOOST=ON \
  -D WUZP_WITH_MP_UNITS=ON -D WUZP_BUILD_TESTS=OFF -D WUZP_BUILD_EXAMPLES=OFF -D CMAKE_INSTALL_PREFIX="${PREFIX}"
cmake --install build/install

# Two builds of the tests, which together use every component of the installed package.
for BACKENDS in "boost eigen" "mp_units blitz"; do
  read -r UNITS VALUES <<< "${BACKENDS}"
  BUILD_DIR="build/consume-tests-${UNITS}-${VALUES}"
  cmake -S tests -B "${BUILD_DIR}" -G Ninja -D CMAKE_PREFIX_PATH="${PREFIX}" -D WUZP_TEST_UNITS="${UNITS}" \
    -D WUZP_TEST_VALUES="${VALUES}" -D WUZP_WARNINGS_AS_ERRORS=ON
  cmake --build "${BUILD_DIR}"
  ctest --test-dir "${BUILD_DIR}" --output-on-failure --no-tests=error
done

cmake -S examples -B build/consume-examples -G Ninja -D CMAKE_PREFIX_PATH="${PREFIX}" -D WUZP_WARNINGS_AS_ERRORS=ON
cmake --build build/consume-examples

# Use wuzp through FetchContent, like the README describes. FETCHCONTENT_SOURCE_DIR_WUZP replaces the download with this
# working tree.
CONSUMER=build/consume-fetchcontent
mkdir -p "${CONSUMER}"
cat > "${CONSUMER}/CMakeLists.txt" <<'CMAKE'
cmake_minimum_required(VERSION 3.22)
project(consumer LANGUAGES CXX)
include(FetchContent)
FetchContent_Declare(wuzp GIT_REPOSITORY https://github.com/remod/wuzp.git GIT_TAG master)
FetchContent_MakeAvailable(wuzp)
add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE wuzp::wuzp wuzp::eigen)
CMAKE
cp examples/quantity.cpp "${CONSUMER}/main.cpp"
cmake -S "${CONSUMER}" -B "${CONSUMER}/build" -G Ninja -D FETCHCONTENT_SOURCE_DIR_WUZP="${PWD}" -D WUZP_WITH_EIGEN=ON
cmake --build "${CONSUMER}/build"
"${CONSUMER}/build/consumer"
