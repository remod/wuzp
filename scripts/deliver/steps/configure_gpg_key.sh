#!/bin/bash

# Exit immediately if a command fails.
set -e

# Do not check arguments, as there are none and this script is sourced, it just gets all arguments of the parent script.
## Check arguments.
#if [ $# != 0 ]; then
#  echo "Number of arguments is incorrect."
#  echo "Usage: ./configure_gpg_key.sh"
#  return 1
#fi

# Configure GPG key.
mkdir /tmp/gnupg
export GNUPGHOME=/tmp/.gnupg
echo -n "${GPG_SIGNING_KEY}" | base64 --decode | gpg --batch --import
echo -n "${GPG_PUBLIC_KEY}" | base64 --decode | gpg --batch --import

return 0
