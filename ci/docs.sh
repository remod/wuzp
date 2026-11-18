#!/usr/bin/env bash
# Builds the documentation site: the API reference with Doxygen into docs/api/ (see Doxyfile), then the pages with MkDocs into
# build/site/ (see mkdocs.yml). MkDocs runs in strict mode, so a broken link or a page missing from the navigation fails the
# build. Preview it with `mkdocs serve` after running this script.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

doxygen --version
mkdocs --version

mkdir -p build
rm -rf docs/api
doxygen Doxyfile
WARNINGS=$(grep -c 'warning:' build/doxygen_warnings.log || true)
if [[ ${WARNINGS} -gt 0 ]]; then
  cat build/doxygen_warnings.log
fi
echo "Doxygen: ${WARNINGS} warnings."

mkdocs build --strict
echo "The site is in build/site."
