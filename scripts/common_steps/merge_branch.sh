#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./merge_branch.sh REPO_DIR BRANCH"
  exit 1
fi

REPO_DIR=${1}
BRANCH=${2}

# Merge branch.
git -C ${REPO_DIR} merge ${BRANCH}

exit 0
