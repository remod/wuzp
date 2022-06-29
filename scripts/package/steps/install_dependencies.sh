#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# -gt 1 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./install_dependencies.sh [WUZP_PACKAGE_TEST_DIR]"
  exit 1
fi

if [ $# -ge 1 ]; then
  WUZP_PACKAGE_PATH="${1}/wuzp*.deb"
fi

# Install dependencies.
apt install -y cmake debhelper-compat devscripts gnupg-agent ${WUZP_PACKAGE_PATH}
# TODO(remod): Make this step cleaner, by only installing the required dependencies.
apt install -y libboost-dev libeigen3-dev libblitz0-dev

exit 0
