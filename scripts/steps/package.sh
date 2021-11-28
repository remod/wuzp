#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 3 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./package.sh SOURCE_DIR BUILD_DIR PACKAGE_DIR"
  exit 1
fi

# Package.
sudo apt install dh-make devscripts dput
export DEBFULLNAME="Remo Diethelm"
export DEBEMAIL="rdiethelm@gmail.com"
cd ${PACKAGE_DIR}
cp -r ${SOURCE_DIR} .
cp ${BUILD_DIR}/Makefile .
dh_make --packageclass s --yes --createorig
debuild -S -sa
cd ..
dput ppa:remod/wuzp wuzp_0.0.1-1_source.changes

exit 0
