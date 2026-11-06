#!/usr/bin/env bash
# Runs the CI jobs locally, in the same container images as CI. Requires Docker.
#
# Usage: ci/local.sh [job...]
#   Jobs: format, test-min, test-latest, coverage, sanitizers, install. Without arguments, all jobs run.
#   WUZP_PRESETS limits the tested presets, e.g. WUZP_PRESETS="gcc-wuzp-eigen clang-boost-blitz" ci/local.sh test-min
#   WUZP_JOBS sets the number of CPU cores to use. The default is half of them, to keep the machine responsive.
#
# Each job runs in a fresh container on a copy of the working tree (without build/), so the working tree is not
# modified. Like CI, a failing job does not stop the others.

set -uo pipefail

REPO_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
ALL_PRESETS="gcc-wuzp-eigen gcc-wuzp-blitz gcc-boost-eigen gcc-boost-blitz clang-wuzp-eigen clang-wuzp-blitz clang-boost-eigen clang-boost-blitz"
PRESETS=${WUZP_PRESETS:-${ALL_PRESETS}}
CORES=$(nproc)
CPUS=${WUZP_JOBS:-$(( CORES > 1 ? CORES / 2 : 1 ))}
echo "Using ${CPUS} of ${CORES} CPU cores (set WUZP_JOBS to change)."

JOBS=("$@")
if [[ ${#JOBS[@]} -eq 0 ]]; then
  JOBS=(format test-min test-latest coverage sanitizers install)
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
  for preset in ${PRESETS}; do
    command+="echo '=== ${preset}'; ci/test.sh ${preset} || failed+=' ${preset}'; "
  done
  command+="if [[ -n \${failed} ]]; then echo \"Failed presets:\${failed}\"; exit 1; fi"
  echo "${command}"
}

declare -A RESULTS
for job in "${JOBS[@]}"; do
  echo "################ ${job}"
  case ${job} in
    format) run_in latest "ci/check_format.sh" ;;
    test-min) run_in min "$(test_command)" ;;
    test-latest) run_in latest "$(test_command)" ;;
    coverage) run_in latest "ci/coverage.sh" ;;
    sanitizers) run_in latest "ci/sanitizers.sh" ;;
    install) run_in latest "ci/install_and_consume.sh" ;;
    *) echo "Unknown job '${job}'." >&2; false ;;
  esac
  if [[ $? -eq 0 ]]; then RESULTS[${job}]=passed; else RESULTS[${job}]=FAILED; fi
done

echo "################ summary"
STATUS=0
for job in "${JOBS[@]}"; do
  echo "${job}: ${RESULTS[${job}]}"
  [[ ${RESULTS[${job}]} == passed ]] || STATUS=1
done
exit ${STATUS}
