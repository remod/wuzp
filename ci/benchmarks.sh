#!/usr/bin/env bash
# Builds the benchmarks (release builds with GCC and Clang) and runs them, then compares the code that the compilers generated
# for wuzp and for plain Eigen (benchmarks/compare_codegen.py). Arguments are passed to the benchmarks, e.g.
# ci/benchmarks.sh --benchmark_min_time=0.01s for a quick run. CI only checks that they build and run: the times on shared
# runners are too noisy to compare.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

export CMAKE_BUILD_PARALLEL_LEVEL=${WUZP_JOBS:-$(getconf _NPROCESSORS_ONLN)}

for PRESET in gcc-benchmarks clang-benchmarks; do
  echo "=== ${PRESET}"
  cmake --preset "${PRESET}" > /dev/null
  cmake --build --preset "${PRESET}"
  for BACKEND in eigen blitz; do
    "build/${PRESET}/benchmarks/wuzp_benchmarks_${BACKEND}" "$@"
  done
  benchmarks/compare_codegen.py "build/${PRESET}"
done
