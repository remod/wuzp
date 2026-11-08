---
paths:
  - "**/*.hpp"
  - "**/*.cpp"
---

<!-- Generated from .cursor/rules/data-types.mdc by .claude/sync-from-cursor.py. Edit the source, then rerun the script. -->

- Use the library's own types in tests and examples: quantities with units, tensors with coordinate systems, points vs. vectors. Avoid raw value matrices where a typed quantity exists.
- Prefer explicit types over `auto` for results of operations: with Eigen, `auto` can hold an expression that refers to a temporary and dangles. If a function returns a result computed from local variables, evaluate it (`.evaluate()`) before returning.
- Make invalid states unrepresentable at compile time, rather than at runtime, and add a compile test:
  - Constrain template parameters with the concepts in `wuzp/core/concepts.hpp` (e.g. `AnyCoordinateSystem`), so that a parameter of the wrong kind fails with a constraint error.
  - Reject invalid combinations of valid types (e.g. matrix sizes that do not match) with a `static_assert` and a clear message.
- Use `requires` clauses instead of `std::enable_if`. For members that only depend on the parameters of their class, e.g. its dimension, use a trailing `requires` on the member instead of a defaulted template parameter.
- Name template parameters after their meaning (`CoordinateSystemT_`, `UnitsT_`), not `T`, `U`.
