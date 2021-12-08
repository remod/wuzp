#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 4 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./package.sh SOURCE_DIR PACKAGE_DIR TARGET_NAME TARGET_VERSION"
  exit 1
fi

SOURCE_DIR=${1}
PACKAGE_DIR=${2}
TARGET_NAME=${3}
TARGET_VERSION=${4}
PACKAGE_WORKSPACE_DIR=${PACKAGE_DIR}/${TARGET_NAME}-${TARGET_VERSION}

sudo aptitude install gnupg-agent pinentry-gtk2 pinentry-curses
export GPGKEY=0xE10ED773
killall -q gpg-agent
eval $(gpg-agent --daemon)

# Package.
export DEBFULLNAME="Remo Diethelm"
export DEBEMAIL="rdiethelm@gmail.com"
mkdir -p ${PACKAGE_DIR}
cp -r ${SOURCE_DIR} ${PACKAGE_WORKSPACE_DIR}
cd ${PACKAGE_WORKSPACE_DIR}
dh_make --packageclass s --createorig --yes && true
debuild -S -sa
cd -

exit 0
