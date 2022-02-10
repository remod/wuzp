#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 4 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./deliver.sh PACKAGE_DIR PACKAGE_NAME PACKAGE_VERSION DRY_RUN"
  exit 1
fi

PACKAGE_DIR=${1}
PACKAGE_NAME=${2}
PACKAGE_VERSION=${3}
DRY_RUN=${4}

# Deliver.
if [ ${DRY_RUN} = true ]; then
  # The '--simulate' argument can be repeated twice to also simulate the network connection.
  # See http://manpages.ubuntu.com/manpages/trusty/man1/dput.1.html for more info.
  OPTIONS="--simulate --simulate"
fi
dput ${OPTIONS} ppa:remod/wuzp ${PACKAGE_DIR}/${PACKAGE_NAME//_/-}_${PACKAGE_VERSION}_source.changes

exit 0
