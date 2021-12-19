#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 3 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./install_packages.sh PACKAGE_DIR UNITS_IMPLEMENTATION VALUE_MATRIX_IMPLEMENTATION"
  exit 1
fi

PACKAGE_DIR=${1}
UNITS_IMPLEMENTATION=${2}
VALUE_MATRIX_IMPLEMENTATION=${3}

# Install packages.
export DEBIAN_FRONTEND=noninteractive
echo "HERE0"
ls ${PACKAGE_DIR}/..
echo "HERE1"
ls ${PACKAGE_DIR}
echo "HERE2"
ls ${PACKAGE_DIR}/wuzp
PACKAGES="${PACKAGE_DIR}/wuzp/*.deb"
if [[ ${UNITS_IMPLEMENTATION} == "1" ]]; then
  PACKAGES="${PACKAGES} ${PACKAGE_DIR}/wuzp_boost/*.deb"
fi
if [[ ${VALUE_MATRIX_IMPLEMENTATION} == "1" ]]; then
  PACKAGES="${PACKAGES} ${PACKAGE_DIR}/wuzp_eigen/*.deb"
fi
dpkg --install ${PACKAGES}

exit 0
