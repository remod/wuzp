#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./folder_changed.sh REPO_DIR FOLDER_DIR"
  return 1
fi

# Do not set variables as this script is sourced.

# Check if the folder changed.
set +e
git -C ${1} diff --quiet --exit-code origin/master -- ${2}
echo ${FOLDER_CHANGED}
set -e

return 0
