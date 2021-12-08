#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 5 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./deliver.sh PACKAGE_DIR TARGET_NAME TARGET_VERSION BUILD_INCREMENT DRY_RUN"
  exit 1
fi

PACKAGE_DIR=${1}
TARGET_NAME=${2}
TARGET_VERSION=${3}
BUILD_INCREMENT=${4}
DRY_RUN=${5}

# Deliver.
if [ ${DRY_RUN} = true ]; then
  # The '--simulate' argument can be repeated twice to also simulate the network connection.
  # See http://manpages.ubuntu.com/manpages/trusty/man1/dput.1.html for more info.
  OPTIONS="--simulate --simulate"
fi
dput ${OPTIONS} ppa:remod/wuzp ${PACKAGE_DIR}/${TARGET_NAME}_${TARGET_VERSION}-${BUILD_INCREMENT}_source.changes

exit 0
