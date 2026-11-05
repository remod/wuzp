#!/usr/bin/env bash
# Installs the build dependencies on Ubuntu (22.04 or newer).
#
# Usage: ci/install_dependencies.sh <gcc|clang> [eigen] [blitz] [boost] [coverage] [format]
#
# The dev containers will replace this script for local development; until then, CI uses it.

set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 <gcc|clang> [eigen] [blitz] [boost] [coverage] [format]" >&2
  exit 1
fi

COMPILER=$1
shift

SUDO=""
if [[ $(id -u) -ne 0 ]]; then
  SUDO="sudo"
fi

PACKAGES=(ca-certificates cmake ninja-build libgtest-dev)
case ${COMPILER} in
  gcc) PACKAGES+=(g++) ;;
  clang) PACKAGES+=(clang) ;;
  *) echo "Unknown compiler '${COMPILER}'." >&2; exit 1 ;;
esac

BUILD_BLITZ_FROM_SOURCE=false
INSTALL_PROFILE_RUNTIME=false
for EXTRA in "$@"; do
  case ${EXTRA} in
    eigen) PACKAGES+=(libeigen3-dev) ;;
    boost) PACKAGES+=(libboost-dev) ;;
    blitz)
      if [[ ${COMPILER} == clang ]]; then
        # The Ubuntu package lacks the configuration header for Clang (https://github.com/blitzpp/blitz/issues/177).
        BUILD_BLITZ_FROM_SOURCE=true
        PACKAGES+=(curl python3)
      else
        PACKAGES+=(libblitz0-dev)
      fi
      ;;
    coverage) PACKAGES+=(llvm) ; INSTALL_PROFILE_RUNTIME=true ;;
    format) PACKAGES+=(clang-format) ;;
    *) echo "Unknown dependency '${EXTRA}'." >&2; exit 1 ;;
  esac
done

export DEBIAN_FRONTEND=noninteractive
${SUDO} apt-get update -qq
# Since Ubuntu 24.04, Clang's profiling runtime is a separate package, which --no-install-recommends skips.
if [[ ${INSTALL_PROFILE_RUNTIME} == true ]] && apt-cache show libclang-rt-dev > /dev/null 2>&1; then
  PACKAGES+=(libclang-rt-dev)
fi
${SUDO} apt-get install -y -qq --no-install-recommends "${PACKAGES[@]}"

if [[ ${BUILD_BLITZ_FROM_SOURCE} == true ]]; then
  BLITZ_VERSION=1.0.2
  WORK_DIR=$(mktemp -d)
  curl -fsSL "https://github.com/blitzpp/blitz/archive/refs/tags/${BLITZ_VERSION}.tar.gz" | tar -xz -C "${WORK_DIR}"
  cmake -S "${WORK_DIR}/blitz-${BLITZ_VERSION}" -B "${WORK_DIR}/build" -G Ninja \
    -D CMAKE_BUILD_TYPE=Release -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++ \
    -D CMAKE_POLICY_VERSION_MINIMUM=3.5  # Blitz++ 1.0.2 declares a CMake minimum that CMake 4 no longer accepts.
  cmake --build "${WORK_DIR}/build" --target lib blitz-static
  ${SUDO} cmake --install "${WORK_DIR}/build"
  rm -rf "${WORK_DIR}"
fi
