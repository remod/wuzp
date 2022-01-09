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
git config --global user.name "Remo Diethelm"
git config --global user.email "rdiethelm@gmail.com"
git -C ${REPO_DIR} fetch origin
git -C ${REPO_DIR} merge --allow-unrelated-histories origin/${BRANCH} ||
git -C ${REPO_DIR} diff

exit 1
