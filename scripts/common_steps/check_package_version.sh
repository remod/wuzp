#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./check_package_version.sh PACKAGE_VERSION_BRANCH PACKAGE_VERSION_MASTER"
  exit 1
fi

PACKAGE_VERSION_BRANCH=${1}
PACKAGE_VERSION_MASTER=${2}

# Check package version.
if [[ "${PACKAGE_VERSION_MASTER}" == "${PACKAGE_VERSION_BRANCH}" ]]; then
  echo "ERROR: The source code has changed, but the package version has not been increased."
  exit 1
fi

exit 0
