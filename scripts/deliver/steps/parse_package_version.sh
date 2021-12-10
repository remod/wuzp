#!/bin/bash

# Exit immediately if a command fails.
set -ex

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
if ! [[ $FIRST_LINE =~ $PATTERN ]]; then
  echo "Failed to parse package version in '${FIRST_LINE}'."
  return 1
fi
export TARGET_VERSION=${BASH_REMATCH[1]}
export BUILD_INCREMENT=${BASH_REMATCH[2]}

return 0
