#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 1 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./parse_package_version.sh SOURCE_DIR"
  return 1
fi

SOURCE_DIR=${1}

# Parse package version.
cd ${SOURCE_DIR}
PACKAGE_VERSION_TMP=$(dpkg-parsechangelog -SVersion)
cd -
export PACKAGE_VERSION=${PACKAGE_VERSION_TMP}

return 0
