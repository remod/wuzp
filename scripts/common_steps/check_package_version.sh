#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 0 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./check_package_version.sh"
  exit 1
fi

# Check package version.
if [[ "${PACKAGE_VERSION_MASTER}" == "${PACKAGE_VERSION_BRANCH}" ]]; then
  echo "ERROR: The source code has changed, but the package version has not been increased."
  exit 1
fi

exit 0
