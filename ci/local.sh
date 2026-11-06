#!/usr/bin/env bash
# Runs the CI jobs locally, in the same container images as CI. Requires Docker.
#
# Usage: ci/local.sh [job...]
#   Jobs: format, tidy, test-min, test-latest, coverage, sanitizers, install, benchmarks. Without arguments, all jobs
#   run.
#   WUZP_PRESETS limits the tested presets, e.g. WUZP_PRESETS="gcc-wuzp-eigen clang-boost-blitz" ci/local.sh test-min
#   WUZP_JOBS sets the number of CPU cores to use. The default is half of them, to keep the machine responsive.
#
# Each job runs in a fresh container on a copy of the working tree (without build/), so the working tree is not
# modified. Like CI, a failing job does not stop the others.

set -uo pipefail

REPO_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
MIN_PRESETS="gcc-wuzp-eigen gcc-wuzp-blitz gcc-boost-eigen gcc-boost-blitz clang-wuzp-eigen clang-wuzp-blitz clang-boost-eigen clang-boost-blitz"
# mp-units needs newer compilers than those of the min variant.
LATEST_PRESETS="${MIN_PRESETS} gcc-mp_units-eigen gcc-mp_units-blitz clang-mp_units-eigen clang-mp_units-blitz"
CORES=$(getconf _NPROCESSORS_ONLN)
CPUS=${WUZP_JOBS:-$(( CORES > 1 ? CORES / 2 : 1 ))}
echo "Using ${CPUS} of ${CORES} CPU cores (set WUZP_JOBS to change)."

JOBS=("$@")
if [[ ${#JOBS[@]} -eq 0 ]]; then
  JOBS=(format tidy test-min test-latest coverage sanitizers install benchmarks)
fi

# Runs a command in a fresh container of the given variant.
run_in() {
  local variant=$1
  local command=$2
  local image
  image=$("${REPO_DIR}/ci/image.sh" ensure "${variant}") || return 1
  # --cpus caps the container, so that nothing inside can use more cores than requested.
  docker run --rm \
    --cpus "${CPUS}" \
    --env "WUZP_JOBS=${CPUS}" \
    --volume "${REPO_DIR}:/src:ro" \
    --volume wuzp-ci-ccache:/ccache \
    "${image}" \
    bash -c "set -euo pipefail
      mkdir /work && tar -C /src --exclude=./build -cf - . | tar -C /work -xf -
      cd /work
      ${command}"
}

# Tests all presets, like the CI matrix: a failing preset does not stop the others.
test_command() {
  local command="failed=''; "
  for preset in ${WUZP_PRESETS:-$1}; do
    command+="echo '=== ${preset}'; ci/test.sh ${preset} || failed+=' ${preset}'; "
  done
  command+="if [[ -n \${failed} ]]; then echo \"Failed presets:\${failed}\"; exit 1; fi"
  echo "${command}"
}

# The results, in the order of JOBS. An indexed array, since macOS's bash 3.2 has no associative arrays.
RESULTS=()
for job in "${JOBS[@]}"; do
  echo "################ ${job}"
  case ${job} in
    format) run_in latest "ci/check_format.sh" ;;
    tidy) run_in latest "ci/tidy.sh" ;;
    test-min) run_in min "$(test_command "${MIN_PRESETS}")" ;;
    test-latest) run_in latest "$(test_command "${LATEST_PRESETS}")" ;;
    coverage) run_in latest "ci/coverage.sh" ;;
    sanitizers) run_in latest "ci/sanitizers.sh" ;;
    install) run_in latest "ci/install_and_consume.sh" ;;
    benchmarks) run_in latest "ci/benchmarks.sh --benchmark_min_time=0.01s" ;;
    *) echo "Unknown job '${job}'." >&2; false ;;
  esac
  if [[ $? -eq 0 ]]; then RESULTS+=(passed); else RESULTS+=(FAILED); fi
done

echo "################ summary"
STATUS=0
for i in "${!JOBS[@]}"; do
  echo "${JOBS[i]}: ${RESULTS[i]}"
  [[ ${RESULTS[i]} == passed ]] || STATUS=1
done
exit ${STATUS}
