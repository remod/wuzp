#!/bin/bash

# Exit immediately if a command fails.
set -e

# Check arguments.
if [ $# != 0 ]; then
  echo "Number of arguments is incorrect."
  echo "Usage: ./install_dependencies.sh"
  exit 1
fi

# Install dependencies.
export DEBIAN_FRONTEND=noninteractive
apt update
apt install -y cmake debhelper-compat devscripts gnupg-agent
# TODO(remod): Make this step cleaner, by only installing the required dependencies.
apt install libboost-dev libeigen3-dev

exit 0
