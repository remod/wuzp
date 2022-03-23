#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 3 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./install_packages.sh PACKAGES_DIR UNITS_IMPLEMENTATION VALUES_IMPLEMENTATION"
  exit 1
fi

PACKAGES_DIR=${1}
UNITS_IMPLEMENTATION=${2}
VALUES_IMPLEMENTATION=${3}

# Install packages.
PACKAGES="${PACKAGES_DIR}/wuzp/*.deb"
if [[ ${UNITS_IMPLEMENTATION} == "1" ]]; then
  PACKAGES="${PACKAGES} ${PACKAGES_DIR}/wuzp_boost/*.deb"
fi
if [[ ${VALUES_IMPLEMENTATION} == "1" ]]; then
  PACKAGES="${PACKAGES} ${PACKAGES_DIR}/wuzp_eigen/*.deb"
fi
# Unlike "dpkg --install", "apt install" automatically installs missing dependencies.
apt install -y ${PACKAGES}

exit 0
