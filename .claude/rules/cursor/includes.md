---
paths:
  - "**/*.hpp"
  - "**/*.cpp"
---

<!-- Generated from .cursor/rules/includes.mdc by .claude/sync-from-cursor.py. Edit the source, then rerun the script. -->

- Every header includes everything it uses, so it compiles on its own. Do not rely on other headers including it first: this breaks with other backends (e.g. Boost.Units or Blitz++ instead of wuzp units and Eigen).
- Includes go at the top of the file: standard library first, then third-party libraries, then wuzp, each block sorted alphabetically.
- Include wuzp headers with their full path: in quotes in the library and the tests (`#include "wuzp/core/Tensor.hpp"`), in angle brackets in the examples and benchmarks, like users do (`#include <wuzp/core/Tensor.hpp>`).
- The core (`wuzp/include/wuzp/`) never includes a backend (`wuzp_eigen/`, `wuzp_blitz/`, `wuzp_boost/`).
