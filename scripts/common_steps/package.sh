#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 4 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./package.sh SOURCE_DIR PACKAGE_DIR PACKAGE_NAME PACKAGE_VERSION"
  exit 1
fi

SOURCE_DIR=${1}
PACKAGE_DIR=${2}
PACKAGE_NAME=${3}
PACKAGE_VERSION=${4}
PACKAGE_WORKSPACE_DIR=${PACKAGE_DIR}/${PACKAGE_NAME//_/-}-${PACKAGE_VERSION}

# Package.
# Configure the debuild.
export DEBFULLNAME="Remo Diethelm"
export DEBEMAIL="rdiethelm@gmail.com"
# Allow signing without interactively entering passphrase.
echo "allow-preset-passphrase" > ${GNUPGHOME}/gpg-agent.conf
gpgconf --kill gpg-agent
eval "$(gpg-agent --daemon)"
/usr/lib/gnupg/gpg-preset-passphrase --preset --passphrase ${GPG_SIGNING_KEY_PASSPHRASE} ${GPG_SIGNING_KEYGRIP}
# Create directory with source copy for out of source packaging.
mkdir -p ${PACKAGE_DIR}
cp -r ${SOURCE_DIR} ${PACKAGE_WORKSPACE_DIR}
# Package.
cd ${PACKAGE_WORKSPACE_DIR}
debuild -sa
cd -

exit 0
