#!/usr/bin/env bash
# Builds the documentation site: the API reference with Doxygen into docs/api/ (see Doxyfile), then the pages with MkDocs into
# build/site/ (see mkdocs.yml). A Doxygen warning fails the build, except the known ones below, and MkDocs runs in strict mode,
# so a broken link or a page missing from the navigation fails it too. To preview the site, see "Documentation" in
# CONTRIBUTING.md.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

# The known warnings, limitations of Doxygen's parser rather than errors of the documentation: Doxygen (1.15 to 1.18) reads a
# constructor with C++20's conditional explicit, `explicit(condition) Class(...)`, as a member function named `explicit`, and
# then finds no matching class member for the constructors of Quantities, Tensor and ValuesWrapper. The line after such a
# warning names the misread member, which this pattern matches.
KNOWN_WARNINGS='^  Ts wuzp::(Quantities|Tensor|ValuesWrapper)::Ts::type$'

doxygen --version
mkdocs --version

mkdir -p build
rm -rf docs/api
doxygen Doxyfile
WARNINGS=$(grep -c ': warning: ' build/doxygen_warnings.log || true)
KNOWN=$(grep -A 1 ': warning: no uniquely matching class member found for' build/doxygen_warnings.log \
  | grep -c -E "${KNOWN_WARNINGS}" || true)
echo "Doxygen: ${WARNINGS} warnings, ${KNOWN} of them known (see KNOWN_WARNINGS in ci/docs.sh)."
if [[ ${WARNINGS} -ne ${KNOWN} ]]; then
  cat build/doxygen_warnings.log
  echo "Doxygen warned about the documentation, see build/doxygen_warnings.log above." >&2
  exit 1
fi
# The Doxyfile's filter escapes the space after abbreviations in the doc comments, e.g. "e.g.\ ", so that they do not end the
# brief descriptions. Where Doxygen prints a comment as written, e.g. in a code span, the escape would show.
SHOWN_ESCAPES=$(grep -rliE '(e\.g|i\.e|eqs?)\.\\ ' docs/api --include='*.html' --exclude='*_source.html' || true)
if [[ -n ${SHOWN_ESCAPES} ]]; then
  echo "${SHOWN_ESCAPES}"
  echo "The API reference shows the escaped spaces of the Doxyfile's filter (INPUT_FILTER) in the pages above." >&2
  exit 1
fi

mkdocs build --strict
echo "The site is in build/site."
