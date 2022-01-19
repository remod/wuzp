#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 2 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./checkout_branch.sh REPO_DIR BRANCH"
  exit 1
fi

REPO_DIR=${1}
BRANCH=${2}

# Checkout branch.
git -C ${REPO_DIR} checkout --force ${BRANCH}

exit 0
