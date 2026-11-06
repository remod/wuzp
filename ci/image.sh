#!/usr/bin/env bash
# Names, pulls and builds the dev container images.
#
# Usage:
#   ci/image.sh name <min|latest>                   Prints the image reference.
#   ci/image.sh ensure <min|latest> [--tag <tag>]   Pulls the image, or builds it if it was not published yet.
#   ci/image.sh publish <min|latest>                Builds and pushes the image, unless it was published already (CI).
#
# Images are tagged with a hash of the Dockerfile and the variant's build arguments, so an image is only rebuilt when
# they change, and local development uses exactly the image CI published. Set WUZP_DEV_IMAGE to use another registry or
# repository.

set -euo pipefail

REPO_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
IMAGE_REPOSITORY=${WUZP_DEV_IMAGE:-ghcr.io/remod/wuzp-dev}

usage() {
  sed -n '2,/^$/p' "${BASH_SOURCE[0]}" >&2
  exit 1
}

# The Dockerfile's build arguments of each variant.
build_args() {
  case $1 in
    min) echo "UBUNTU_VERSION=22.04 EIGEN_FROM_SOURCE=false" ;;
    latest) echo "UBUNTU_VERSION=26.04 EIGEN_FROM_SOURCE=true" ;;
    *) echo "Unknown variant '$1', expected 'min' or 'latest'." >&2; exit 1 ;;
  esac
}

image_name() {
  local variant=$1
  local args hash
  args=$(build_args "${variant}")
  hash=$( (cat "${REPO_DIR}/.devcontainer/Dockerfile"; echo "${args}") | sha256sum | cut -c1-16)
  echo "${IMAGE_REPOSITORY}:${variant}-${hash}"
}

build_image() {
  local variant=$1
  local image=$2
  local args=() arg
  for arg in $(build_args "${variant}"); do
    args+=(--build-arg "${arg}")
  done
  echo "Building ${image}." >&2
  # The build output goes to stderr, so that stdout only contains the image reference.
  docker build "${args[@]}" --tag "${image}" "${REPO_DIR}/.devcontainer" >&2
}

[[ $# -ge 2 ]] || usage
COMMAND=$1
VARIANT=$2
shift 2

case ${COMMAND} in
  name)
    image_name "${VARIANT}"
    ;;
  ensure)
    LOCAL_TAG=""
    while [[ $# -gt 0 ]]; do
      case $1 in
        --tag) LOCAL_TAG=$2; shift 2 ;;
        *) usage ;;
      esac
    done
    IMAGE=$(image_name "${VARIANT}")
    if docker image inspect "${IMAGE}" > /dev/null 2>&1; then
      echo "Using local image ${IMAGE}." >&2
    elif docker pull --quiet "${IMAGE}" > /dev/null 2>&1; then
      echo "Pulled ${IMAGE}." >&2
    else
      echo "${IMAGE} is not published yet, or the registry is not accessible." >&2
      build_image "${VARIANT}" "${IMAGE}"
    fi
    if [[ -n ${LOCAL_TAG} ]]; then
      docker tag "${IMAGE}" "${LOCAL_TAG}"
    fi
    echo "${IMAGE}"
    ;;
  publish)
    IMAGE=$(image_name "${VARIANT}")
    if docker manifest inspect "${IMAGE}" > /dev/null 2>&1; then
      echo "${IMAGE} is already published." >&2
    else
      build_image "${VARIANT}" "${IMAGE}"
      docker push "${IMAGE}" >&2
    fi
    echo "${IMAGE}"
    ;;
  *)
    usage
    ;;
esac
