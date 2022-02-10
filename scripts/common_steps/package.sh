#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 5 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./package.sh SOURCE_DIR PACKAGE_TEST_DIR PACKAGE_SHIP_DIR PACKAGE_NAME PACKAGE_VERSION"
  exit 1
fi

SOURCE_DIR=${1}
PACKAGE_TEST_DIR=${2}
PACKAGE_SHIP_DIR=${3}
PACKAGE_NAME=${4}
PACKAGE_VERSION=${5}
PACKAGE_TEST_WORKSPACE_DIR=${PACKAGE_TEST_DIR}/${PACKAGE_NAME//_/-}-${PACKAGE_VERSION}
PACKAGE_SHIP_WORKSPACE_DIR=${PACKAGE_SHIP_DIR}/${PACKAGE_NAME//_/-}-${PACKAGE_VERSION}

# Package.
# Configure debuild.
export DEBFULLNAME="Remo Diethelm"
export DEBEMAIL="rdiethelm@gmail.com"
# Allow signing without interactively entering passphrase.
echo "allow-preset-passphrase" > ${GNUPGHOME}/gpg-agent.conf
gpgconf --kill gpg-agent
eval "$(gpg-agent --daemon)"
/usr/lib/gnupg/gpg-preset-passphrase --preset --passphrase ${GPG_SIGNING_KEY_PASSPHRASE} ${GPG_SIGNING_KEYGRIP}
# Package for testing.
# Create directory with source copy for out of source packaging.
mkdir -p ${PACKAGE_TEST_DIR}
cp -r ${SOURCE_DIR} ${PACKAGE_TEST_WORKSPACE_DIR}
cd ${PACKAGE_TEST_WORKSPACE_DIR}
debuild -b -sa
cd -
# Package for shipping.
# Create directory with source copy for out of source packaging.
mkdir -p ${PACKAGE_SHIP_DIR}
cp -r ${SOURCE_DIR} ${PACKAGE_SHIP_WORKSPACE_DIR}
cd ${PACKAGE_SHIP_WORKSPACE_DIR}
debuild -S -sa
cd -

exit 0
