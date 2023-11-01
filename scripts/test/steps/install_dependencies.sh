#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 4 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./install_dependencies.sh CMAKE_C_COMPILER CMAKE_CXX_COMPILER UNITS_IMPLEMENTATION VALUES_IMPLEMENTATION"
  exit 1
fi

CMAKE_C_COMPILER=${1}
CMAKE_CXX_COMPILER=${2}
UNITS_IMPLEMENTATION=${3}
VALUES_IMPLEMENTATION=${4}

# Install dependencies.
DEPENDENCIES="build-essential cmake gcc g++ clang libgtest-dev clang-tidy llvm"
if [[ ${UNITS_IMPLEMENTATION} == "1" ]]; then
  DEPENDENCIES="${DEPENDENCIES} libboost-dev"
fi
if [[ ${VALUES_IMPLEMENTATION} == "1" ]]; then
  DEPENDENCIES="${DEPENDENCIES} libeigen3-dev"
elif [[ ${VALUES_IMPLEMENTATION} == "2" ]]; then
  if [[ ${CMAKE_C_COMPILER} == "clang" ]]; then
    # With clang we need to install Blitz from source from https://github.com/blitzpp/blitz/archive/refs/tags/1.0.2.zip
    sudo apt install unzip wget
    wget https://github.com/blitzpp/blitz/archive/refs/tags/1.0.2.zip -P /tmp/blitz
    # Unpack
    unzip /tmp/blitz/1.0.2.zip -d /tmp
    # Build
    mkdir /tmp/blitz-1.0.2/build
    cd /tmp/blitz-1.0.2/build
    cmake ..
    make lib
    # Install
    sudo make install
  else
    DEPENDENCIES="${DEPENDENCIES} libblitz0-dev"
  fi
fi
apt install -y ${DEPENDENCIES}

exit 0
