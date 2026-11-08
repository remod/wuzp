---
paths:
  - "tests/**"
  - "wuzp*/include/**"
---

<!-- Generated from .cursor/rules/testing.mdc by .claude/sync-from-cursor.py. Edit the source, then rerun the script. -->

- Tests are deterministic. Do not use `random()` values unless the test checks a property that holds for every value.
- Every test runs against all backend combinations. Write tests against the backend-agnostic test headers (`tests/src/Unit.hpp`, `ValueMatrix.hpp`); use `#if UNITS_IMPLEMENTATION == ...` only where the backends really differ, and explain why.
- Every check that rejects invalid code needs a compile test in `tests/compile_tests/cases/` that names the expected message. See `tests/compile_tests/CompileTest.cmake` for the annotations.
  - A `static_assert`: its message. `compile.diagnostics_coverage` enforces this.
  - A concept constraint: the constraint error, `// wuzp-error: constraints?( are)? (failure|not satisfied)` (GCC, Clang and MSVC word it differently). Since the error does not name the concept, the case must only violate the constraint it tests.
- Compiler-specific expectations of compile tests cover every tested compiler: `gnu`, `clang`, `appleclang` (macOS) and `msvc` (Windows). While the repository is private, the macOS and Windows CI jobs only run when the workflow is started manually with `platforms`, so a missing expectation goes unnoticed otherwise.
- Bug fixes come with a test that fails without the fix.
- Compare floating point results with `isApprox` and an explicit precision, exact values with `EXPECT_EQ`.
- Never lower a coverage threshold, add a coverage exclusion, or weaken or delete a test without asking the user first.
