#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 0 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./set_locale.sh"
  exit 1
fi

# Set locale.
locale-gen de_DE.UTF-8
update-locale LANG=en_US.UTF-8

exit 0
