#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 0 ]; then
  echo "Number of arguments is incorrect: $*"
  echo "Usage: ./upgrade.sh"
  exit 1
fi

# Upgrade.
apt update
apt upgrade -y

exit 0
