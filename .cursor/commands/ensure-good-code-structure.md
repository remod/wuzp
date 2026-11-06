Review recent changes against this project's layout and rules (`.cursor/rules/`):

- Core logic in `wuzp/include/wuzp/core/` and `trafo/`, independent of any backend; backends (`wuzp_eigen/`, `wuzp_blitz/`, `wuzp_boost/`, `wuzp_mp_units/`, `wuzp_units/`) only adapt another library to the core's traits.
- Every header includes what it uses and compiles on its own.
- Behavior that differs per backend is selected through trait specializations, not `#if`.
- Rules that reject invalid code use `static_assert` with a clear message, close to where the user's mistake happens.
- Tests in `tests/src/` (unit tests) and `tests/compile_tests/cases/` (compile tests), reusing the shared test headers.

Ensure the change fits existing patterns before introducing new abstractions. Check that `README.md`, `CONTRIBUTING.md` and `DESIGN.md` still describe the code, and that `CHANGELOG.md` lists changes users notice.
