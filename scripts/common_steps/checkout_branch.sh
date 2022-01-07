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
git -C ${REPO_DIR} remote update
git -C ${REPO_DIR} reset --hard
git -C ${REPO_DIR} checkout ${BRANCH}
git -C ${REPO_DIR} pull ${BRANCH}

exit 0
