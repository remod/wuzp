#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./copy_folder.sh SOURCE_DIR TARGET_DIR"
  exit 1
fi

SOURCE_DIR=${1}
TARGET_DIR=${2}

# Copy folder.
rm -rf ${TARGET_DIR}
mkdir -p ${TARGET_DIR}
cp -r ${SOURCE_DIR}/. ${TARGET_DIR}

exit 0
