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
apt install -y clang-format git

exit 0
