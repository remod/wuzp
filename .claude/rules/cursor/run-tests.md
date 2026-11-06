---
paths:
  - "**/*.hpp"
  - "**/*.cpp"
  - "**/CMakeLists.txt"
  - "**/*.cmake"
  - "CMakePresets.json"
  - "ci/**"
  - ".devcontainer/**"
---

<!-- Generated from .cursor/rules/run-tests.mdc by .claude/sync-from-cursor.py. Edit the source, then rerun the script. -->

After changing C++ or CMake code, run the CI jobs locally, in the same containers as CI:

1. While iterating, a few presets: `WUZP_PRESETS="gcc-wuzp-eigen clang-boost-blitz" ci/local.sh test-latest`.
2. Before finishing: `ci/local.sh` (format, all presets on `min` and `latest`, coverage, sanitizers, install).

Inside the dev container, the CMake presets work directly: `cmake --preset gcc-wuzp-eigen && cmake --build --preset gcc-wuzp-eigen && ctest --preset gcc-wuzp-eigen`.
