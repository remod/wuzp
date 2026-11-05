For recent changes:

1. Add unit tests in `tests/src/` for new or changed behavior, written against the backend-agnostic test headers so they run with every backend combination.
2. Add a compile test in `tests/compile_tests/cases/` for every new `static_assert`, naming the expected message (see `tests/compile_tests/CompileTest.cmake`). `compile.diagnostics_coverage` fails otherwise.
3. Run `ci/local.sh coverage` and check the report for uncovered lines, regions and branches in the changed headers. Add tests for the gaps.

Never lower coverage, add exclusions, or weaken tests without asking the user.
