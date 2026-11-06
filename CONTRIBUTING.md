# Contributing to wuzp

Thank you for helping! Bug reports, questions and pull requests are welcome.

## Issues

Report bugs and propose features in the [issue tracker](https://github.com/remod/wuzp/issues), using its templates.
For a bug, a minimal program that shows it, together with the compiler and the backends you use, helps the most.

## Pull requests

- Keep a pull request to one topic, and open an issue first for larger changes.
- Add unit tests for new behavior, and compile tests for code that must not compile (see [Compile tests](#compile-tests)).
  A bug fix comes with a test that fails without it.
- Every feature works with every backend combination: the tests run against all of them.
- Run `ci/local.sh` before pushing (see [Continuous integration](#continuous-integration)).
- Add a line to the `Unreleased` section of [CHANGELOG.md](CHANGELOG.md) for changes users notice.
- Commit messages start with one sentence in the imperative that ends with a period, e.g. "Fix the units of TimePoint.", followed by a body that explains why.

## Development environment

Development happens in a [dev container](https://containers.dev/), the same container image that CI uses, so local builds and CI see the same compilers, libraries and tools.
There are two variants:

* `min`: Ubuntu 22.04 with the oldest supported dependency versions (GCC 11, Clang 14, CMake 3.22, Eigen 3.4, Boost 1.74).
* `latest`: Ubuntu 26.04 with the newest supported dependency versions (GCC 15, Clang 21, CMake 4.2, Eigen 5.0, Boost 1.90).

Open the repository in VS Code or CLion and choose *Reopen in Container*, or use the [Dev Container CLI](https://github.com/devcontainers/cli):

```bash
devcontainer up --workspace-folder . --config .devcontainer/latest/devcontainer.json
devcontainer exec --workspace-folder . --config .devcontainer/latest/devcontainer.json bash
```

The images are published by CI.
If the Dockerfile or its build arguments in `ci/image.sh` were changed locally, the image is built locally instead.

## Building and testing

The test suite runs against every combination of units backend and value matrix backend, to make sure *wuzp* does not depend on one particular implementation.
Each combination has a CMake preset named `<compiler>-<units>-<values>`, e.g. `gcc-wuzp-eigen` or `clang-boost-blitz` (see `cmake --list-presets`):

```bash
cmake --preset gcc-wuzp-eigen
cmake --build --preset gcc-wuzp-eigen
ctest --preset gcc-wuzp-eigen
```

## Compile tests

Type safety means that invalid code must not compile, and the error must explain why.
Compile tests verify this: each file in `tests/compile_tests/cases` is a small program, annotated with the outcome it expects:

```cpp
// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

#include "wuzp/core/Quantities.hpp"
// ...
```

A case that must fail to compile always names the expected error, as a regex (`wuzp-error`) or as a `static_assert` message (`wuzp-static-assert`).
Expectations can depend on the configuration, e.g. `// wuzp-error(units=boost,compiler=clang): ...`, and `// wuzp-requires: values=eigen` limits a case to some configurations.
See `tests/compile_tests/CompileTest.cmake` for the details.
Compile tests are labelled `compile`, so `ctest --preset <preset> -L compile` runs only them.

Every `static_assert` message in the headers needs a compile test.
The test `compile.diagnostics_coverage` enforces this; messages that are not tested yet are listed in `tests/compile_tests/untested_diagnostics.txt`, and that list must only shrink.

## Formatting and tidying

The code is formatted using [clang-format](https://clang.llvm.org/docs/ClangFormat.html), with the version pinned in the dev container, and checked with [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) (configured in `.clang-tidy`), for every backend combination:

```bash
cmake --build --preset gcc-wuzp-eigen --target wuzp_format
ci/tidy.sh                  # All backend combinations, or e.g. ci/tidy.sh clang-wuzp-eigen.
```

To format the code automatically before each commit, enable the [pre-commit](https://pre-commit.com/) hook with `pre-commit install`.

## Continuous integration

The CI workflow checks the format, runs clang-tidy, runs the tests for all backend combinations with GCC and Clang in both dev container variants, runs them with AddressSanitizer and UndefinedBehaviorSanitizer, measures code coverage, tests the installed package and the use through `FetchContent`, and builds and runs the [benchmarks](benchmarks/README.md).
Warnings are errors in CI (`WUZP_WARNINGS_AS_ERRORS`, on in all presets), and every public header is compiled on its own with strict warnings.
Each job is a script in `ci/`, and `ci/local.sh` runs the same jobs locally in the same containers (requires Docker):

```bash
ci/local.sh                 # All jobs.
ci/local.sh format test-min # Selected jobs: format, tidy, test-min, test-latest, coverage, sanitizers, install, benchmarks.
```

By default, `ci/local.sh` uses half of the CPU cores; set `WUZP_JOBS` to change this.

Codecov checks that the coverage does not drop and that new code is covered (`codecov.yml`).

## Versioning

*wuzp* follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
The public API consists of the headers in `wuzp/include/`, `wuzp_eigen/include/`, `wuzp_blitz/include/` and `wuzp_boost/include/`, except those in `internal/` directories, and of the CMake package and targets.
Breaking changes need a new major version, and are listed as such in [CHANGELOG.md](CHANGELOG.md).

The oldest supported dependency versions are those of the oldest Ubuntu LTS release in standard support, currently Ubuntu 22.04 (see the `min` dev container).
When that release reaches the end of its standard support, the next minor version raises the minimum versions.

## Releasing

1. Move the `Unreleased` entries in [CHANGELOG.md](CHANGELOG.md) to a new section `## [<version>] - <date>`, add its link to the release at the end of the file, point the `Unreleased` link to the changes since that release, and set the version in `CMakeLists.txt`.
2. Once CI passed on that commit, push a tag `v<version>`, e.g. `v1.0.0`.
   The release workflow checks that the tag matches the version in `CMakeLists.txt`, and creates a GitHub release with the changelog section as release notes.
