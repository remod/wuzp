#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 5 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./package.sh SOURCE_DIR PACKAGE_DIR TARGET_NAME TARGET_VERSION GNUPG_DIR"
  exit 1
fi

SOURCE_DIR=${1}
PACKAGE_DIR=${2}
TARGET_NAME=${3}
TARGET_VERSION=${4}
GNUPG_DIR=${5}
PACKAGE_WORKSPACE_DIR=${PACKAGE_DIR}/${TARGET_NAME}-${TARGET_VERSION}

# Package.
export DEBFULLNAME="Remo Diethelm"
export DEBEMAIL="rdiethelm@gmail.com"
mkdir -p ${PACKAGE_DIR}
cp -r ${SOURCE_DIR} ${PACKAGE_WORKSPACE_DIR}
cd ${PACKAGE_WORKSPACE_DIR}
echo ${PWD}
ls
dh_make --packageclass s --createorig --yes && true
export GNUPGHOME=${GNUPG_DIR}
debuild -S -sa
cd -

exit 0
