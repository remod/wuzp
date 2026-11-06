#!/usr/bin/env bash
# Prints the section of a version in CHANGELOG.md, which the release workflow uses as release notes.
#
# Usage: ci/release_notes.sh <version>, e.g. ci/release_notes.sh 1.0.0
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

[[ $# -eq 1 ]] || { echo "Usage: $0 <version>" >&2; exit 1; }
VERSION=$1

# The section starts with its heading "## [<version>]" and ends before the next heading or the link definitions.
NOTES=$(awk -v heading="## [${VERSION}]" '
  index($0, heading) == 1 { found = 1; next }
  found && (/^## / || /^\[[^]]*\]: /) { exit }
  found { print }
' CHANGELOG.md | sed -e '/./,$!d')

if [[ -z ${NOTES//[[:space:]]/} ]]; then
  echo "CHANGELOG.md has no section for version ${VERSION}." >&2
  exit 1
fi
echo "${NOTES}"
