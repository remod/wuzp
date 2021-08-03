#!/bin/bash

# Exit immediately if a command fails.
set -E

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./install_dependencies.sh UNITS_IMPLEMENTATION MATRIX_IMPLEMENTATION"
  exit 1
fi

# Install dependencies.
export DEBIAN_FRONTEND=noninteractive
apt update
DEPENDENCIES="git cmake build-essential libgtest-dev clang-format clang-tidy"
if [[ "${1}" == "1" ]]; then
  DEPENDENCIES="${DEPENDENCIES} libboost-dev"
fi
if [[ "${2}" == "1" ]]; then
  DEPENDENCIES="${DEPENDENCIES} libeigen3-dev"
fi
apt install -y ${DEPENDENCIES}
