#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 1 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./parse_package_version.sh SOURCE_DIR"
  return 1
fi

# Do not set variables as this script is sourced.

# Parse package version.
cd ${1}
PACKAGE_VERSION_TMP=$(dpkg-parsechangelog -SVersion)
cd -
export PACKAGE_VERSION=${PACKAGE_VERSION_TMP}

return 0
