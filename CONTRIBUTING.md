# Contributing to wuzp

Thank you for helping! Bug reports, questions and pull requests are welcome.
Everyone taking part follows the [code of conduct](CODE_OF_CONDUCT.md).

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
* `latest`: Ubuntu 26.04 with the newest supported dependency versions (GCC 15, Clang 21, CMake 4.2, Eigen 5.0, Boost 1.90, mp-units 2.5).
  mp-units is only tested here: it needs newer compilers than those of `min`.

Open the repository in VS Code or CLion and choose *Reopen in Container*, or use the [Dev Container CLI](https://github.com/devcontainers/cli):

```bash
devcontainer up --workspace-folder . --config .devcontainer/latest/devcontainer.json
devcontainer exec --workspace-folder . --config .devcontainer/latest/devcontainer.json bash
```

The images are published by CI.
If the Dockerfile or its build arguments in `ci/image.sh` were changed locally, the image is built locally instead.

## Building and testing

The test suite runs against every combination of units backend and value matrix backend, to make sure *wuzp* does not depend on one particular implementation.
Each combination has a CMake preset named `<compiler>-<units>-<values>`, e.g. `gcc-wuzp-eigen` or `clang-boost-blitz` (see `cmake --list-presets`).
Building the tests needs both Eigen and Blitz++, whatever the preset: `tests/src/ValueMatrixBackends.cpp` uses both value matrix backends in one translation unit, and every preset enables both.

```bash
cmake --preset gcc-wuzp-eigen
cmake --build --preset gcc-wuzp-eigen
ctest --preset gcc-wuzp-eigen
```

The test presets build in debug mode, so the tests also cover the checks that only run in debug builds, i.e. if `NDEBUG` is not defined, e.g. the validity checks of rotations and Lorentz transformations.
The preset `gcc-wuzp-eigen-release` runs the tests in release mode, without these checks and with the warnings that only the optimizer finds; it skips the compile tests, which do not depend on the build type.
The benchmarks build in release mode too.

On macOS, the `clang-*` presets use Apple Clang.
On Windows, the `msvc-*` presets use MSVC from a developer command prompt.
The `macos` and `windows` jobs in `.github/workflows/ci.yml` show how to get the dependencies with Homebrew and vcpkg.

## Backends

A backend adapts another library by specializing the traits of the core (`wuzp/core/traits.hpp`): a value matrix backend for the matrices and expressions of a linear algebra library, constrained to them, and a units backend for the units of a units library.
The concepts `ValueMatrixBackend`, `InvertibleValueMatrixBackend` and `UnitSystem` in `wuzp/core/concepts.hpp` define what the core needs, and the tests check that every backend satisfies its concept with a `static_assert` (`tests/src/ValueMatrixBackends.cpp`, `tests/src/Unit.cpp`), rather than its header, where the check would cost compile time in every translation unit of the users (see "The backend interface" in [DESIGN.md](DESIGN.md#the-backend-interface)).

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
Expectations can depend on the configuration, e.g. `// wuzp-error(units=boost,compiler=clang|appleclang): ...`, and `// wuzp-requires: values=eigen` limits a case to some configurations.
See `tests/compile_tests/CompileTest.cmake` for the details.
Compile tests are labelled `compile`, so `ctest --preset <preset> -L compile` runs only them.

Every `static_assert` message in the headers needs a compile test.
The test `compile.diagnostics_coverage` enforces this; messages that are not tested yet are listed in `tests/compile_tests/untested_diagnostics.txt`, and that list must only shrink.
Every concept constraint on a template parameter needs a compile test too.
Compilers word constraint errors differently, so such a case expects `// wuzp-error: constraints?( are)? (failure|not satisfied)`, and only violates the constraint it tests.

## Formatting and tidying

The code is formatted using [clang-format](https://clang.llvm.org/docs/ClangFormat.html), with the version pinned in the dev container, and checked with [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) (configured in `.clang-tidy`), for every backend combination:

```bash
cmake --build --preset gcc-wuzp-eigen --target wuzp_format
ci/tidy.sh                  # All backend combinations, or e.g. ci/tidy.sh clang-wuzp-eigen.
```

To format the code automatically before each commit, enable the [pre-commit](https://pre-commit.com/) hook with `pre-commit install`.

## Continuous integration

The CI workflow checks the format, runs clang-tidy, runs the tests for all backend combinations with GCC and Clang in both dev container variants (mp-units only in `latest`) and for one in release mode, runs them with AddressSanitizer and UndefinedBehaviorSanitizer, measures code coverage, tests the installed package and the use through `FetchContent`, and builds and runs the [benchmarks](benchmarks/README.md).
It also tests every backend combination except mp-units on macOS with Apple Clang (Homebrew packages) and on Windows with MSVC (vcpkg packages).
While the repository is private, these two jobs only run when the workflow is started manually with the `platforms` input, since GitHub bills their minutes at 10 and 2 times the Linux rate.
Warnings are errors in CI (`WUZP_WARNINGS_AS_ERRORS`, on in all presets), and every public header is compiled on its own with strict warnings.
Each job is a script in `ci/`, and `ci/local.sh` runs the Linux jobs locally in the same containers (requires Docker):

```bash
ci/local.sh                 # All jobs.
ci/local.sh format test-min # Selected jobs: format, tidy, test-min, test-latest, coverage, sanitizers, install, benchmarks.
```

`test-latest` also runs `gcc-wuzp-eigen-release`, the preset of the CI job `test-release`.
By default, `ci/local.sh` uses half of the CPU cores; set `WUZP_JOBS` to change this.

Codecov checks that the coverage does not drop and that new code is covered (`codecov.yml`).

## Versioning

*wuzp* follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
The public API consists of the headers in `wuzp/include/`, `wuzp_eigen/include/`, `wuzp_blitz/include/`, `wuzp_boost/include/` and `wuzp_mp_units/include/`, except those in `internal/` directories, and of the CMake package and targets.
Breaking changes need a new major version, and are listed as such in [CHANGELOG.md](CHANGELOG.md).

The oldest supported compiler and dependency versions are those of Ubuntu 22.04 (see the `min` dev container).
mp-units is an exception: Ubuntu does not package it, and it needs newer compilers than those of Ubuntu 22.04. It is supported from version 2.5, the one in the `latest` dev container.
Raising a minimum compiler or dependency version breaks the build for some users, so it is a breaking change too and only happens in a new major version.

## Releasing

1. Move the `Unreleased` entries in [CHANGELOG.md](CHANGELOG.md) to a new section `## [<version>] - <date>`, add its link to the release at the end of the file, point the `Unreleased` link to the changes since that release, and set the version in `CMakeLists.txt`.
2. Once CI passed on that commit, push a tag `v<version>`, e.g. `v1.0.0`.
   The release workflow checks that the tag matches the version in `CMakeLists.txt`, and creates a GitHub release with the changelog section as release notes.
