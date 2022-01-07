#!/bin/bash

# Exit immediately if a command fails.
#set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./folder_changed.sh REPO_DIR FOLDER_DIR"
  return 1
fi

# Do not set variables as this script is sourced.

# Check if the folder changed.
git -C ${1} remote update
(git -C ${1} diff --exit-code origin/master -- ${2})
export FOLDER_CHANGED=$?

return 0
