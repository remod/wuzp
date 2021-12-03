#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 1 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./parse_package_version.sh SOURCE_DIR"
  return 1
fi

SOURCE_DIR=${1}

# Parse package version.
PATTERN='\((.*)-(.*)\)'
FIRST_LINE=$(head -n 1 ${SOURCE_DIR}/debian/changelog)
[[ $FIRST_LINE =~ $PATTERN ]]
export TARGET_VERSION=${BASH_REMATCH[1]}
export BUILD_INCREMENT=${BASH_REMATCH[2]}

return 0
