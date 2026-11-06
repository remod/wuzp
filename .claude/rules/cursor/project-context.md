<!-- Generated from .cursor/rules/project-context.mdc by .claude/sync-from-cursor.py. Edit the source, then rerun the script. -->

*wuzp* ("Wrong Unit? Zero Points!") is a header-only C++20 library that adds compile-time checks for units, coordinate systems and absolute/relative quantities (points vs. vectors) to linear algebra libraries, plus active and passive transformations.

- `wuzp/include/wuzp/core/`: units, quantities, tensors, coordinate systems, traits. `wuzp/include/wuzp/trafo/`: transformations. `wuzp/include/wuzp_units/`: wuzp's own units backend.
- `wuzp_eigen/`, `wuzp_blitz/` (value matrix backends) and `wuzp_boost/` (units backend) adapt other libraries.
- `tests/src/`: unit tests (GoogleTest). `tests/compile_tests/cases/`: compile tests (code that must or must not compile). `examples/`: examples.
- `ci/`: one script per CI job, `ci/local.sh` runs them locally. `.devcontainer/`: the dev container images used locally and in CI.
- `README.md`: usage. `CONTRIBUTING.md`: development environment, tests, CI, versioning and releases. `CHANGELOG.md`: user-visible changes. `DESIGN.md`: conventions, review results and open design questions (TODOs).

The backends exist to prove that the concepts generalize and do not overfit to one implementation. Every feature must work with every backend combination, and the tests run against all of them. Stay backwards compatible with the supported dependency versions (the `min` dev container has the oldest).
