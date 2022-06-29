#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./install_dependencies.sh UNITS_IMPLEMENTATION VALUES_IMPLEMENTATION"
  exit 1
fi

UNITS_IMPLEMENTATION=${1}
VALUES_IMPLEMENTATION=${2}

# Install dependencies.
DEPENDENCIES="build-essential cmake gcc g++ clang libgtest-dev clang-tidy llvm"
if [[ ${UNITS_IMPLEMENTATION} == "1" ]]; then
  DEPENDENCIES="${DEPENDENCIES} libboost-dev"
fi
if [[ ${VALUES_IMPLEMENTATION} == "1" ]]; then
  DEPENDENCIES="${DEPENDENCIES} libeigen3-dev"
elif [[ ${VALUES_IMPLEMENTATION} == "2" ]]; then
  DEPENDENCIES="${DEPENDENCIES} libblitz0-dev"
fi
apt install -y ${DEPENDENCIES}

exit 0
