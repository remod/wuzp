<!-- Generated from .cursor/rules/separation-of-concerns.mdc by .claude/sync-from-cursor.py. Edit the source, then rerun the script. -->

Follow separation of concerns:

- **`wuzp/include/wuzp/core/`**: units, quantities, tensors and their compile-time rules. Independent of any backend.
- **`wuzp/include/wuzp/trafo/`**: transformations, built on the core.
- **`wuzp/include/wuzp_units/`, `wuzp_boost/`**: units backends. **`wuzp_eigen/`, `wuzp_blitz/`**: value matrix backends. Backends only adapt another library to the traits of the core; they contain no domain logic.
- **`tests/src/`**: unit tests, **`tests/compile_tests/`**: compile tests, **`examples/`**: user-facing examples.
- **`ci/`**: CI job scripts, **`.devcontainer/`**: the build environment, **`cmake/`**: CMake package files and build helpers (e.g. the warning options).

Backend-specific behavior is selected through trait specializations, never with `#if` in the core.
