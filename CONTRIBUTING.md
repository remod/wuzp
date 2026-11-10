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
Each combination has a CMake preset named `<compiler>-<units>-<values>`, e.g. `gcc-wuzp-eigen` or `clang-boost-blitz` (see `cmake --list-presets`), and the presets `gcc-archetype-archetype` and `clang-archetype-archetype` run it against the archetypes, minimal backends of the tests (see [Writing a backend](#writing-a-backend)).
Building the tests needs both Eigen and Blitz++, whatever the preset: `tests/src/ValueMatrixBackends.cpp` uses both value matrix backends and the archetype in one translation unit, and every preset enables both.

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

## Writing a backend

A backend adapts another library to the core by specializing the traits of `wuzp/core/traits.hpp`: a value matrix backend adapts a linear algebra library, like `wuzp_eigen` and `wuzp_blitz`, and a units backend a units library, like `wuzp_units`, `wuzp_boost` and `wuzp_mp_units`.
Backends contain no domain logic: the core checks units, coordinate systems, indices and the rules of points and vectors, the backends only compute.
Behavior that depends on the library is selected by these trait specializations, never with `#if` in the core.

The interface is defined by [C++20 concepts](https://en.cppreference.com/w/cpp/language/constraints) in `wuzp/core/concepts.hpp`: `ValueMatrixBackend` and `InvertibleValueMatrixBackend` for value matrix backends, `UnitSystem` for units backends (see "The backend interface" in [DESIGN.md](DESIGN.md#the-backend-interface)).
The concepts check the syntax: that the traits exist for representative matrices and units, and that their results have the right types.
Their semantics, e.g. that a copy does not share the values of the original, are documented with the traits and checked by the test suite.

The minimal examples are the *archetypes* of the tests, `tests/src/archetype_values/ValueMatrix.hpp` and `tests/src/archetype_units/Unit.hpp`.
Like the [archetypes of Boost's Concept Check library](https://www.boost.org/doc/libs/release/libs/concept_check/doc/reference.htm#basic-archetype), they provide exactly what the concepts require and nothing more: a fixed-size array without copy or assignment, with naive loops and eager evaluation, and units with integer exponents of time, length and mass.
The presets `gcc-archetype-archetype` and `clang-archetype-archetype` run the whole test suite against them, so it fails if the core uses anything that the concepts do not promise.
The archetypes are test code: they are not installed, and not part of the public API.

### Value matrix backends

A value matrix backend:

* Defines the alias template `ValueMatrix<ValueT_, RowsV_, ColsV_>`, the `wuzp::ValuesWrapper<wuzp::MatrixSize<RowsV_, ColsV_>, ValueT_, Matrix>` of the matrices of its library, which the tensor aliases and transformations take as a parameter.
  Like the other backends, it also defines the aliases that the tests use, e.g. `ValueMatrixD`, `Value` and `QuantityD`.
* Specializes the traits for `wuzp::AccessImplementation<ImplementationT_>`, where `ImplementationT_` is a matrix or an expression of its library, its *implementation types*, except `ReserveMemory` and `Initialize`, which take the matrix type itself.
  It constrains the specializations to them, e.g. `template <EigenArray ImplementationT_>`, so that several value matrix backends can be used in one program, also in one translation unit.
* Provides, for every arithmetic value type and size: allocation (a default constructor and `ReserveMemory`), initialization with the elements, row by row, as values of the value type (`Initialize`), evaluation (`Evaluate` and `Rebind`), copies and moves (`Copy` and `Move`), element access (`Coefficient`), `Zero`, `Random` (uniform in `[-1, 1]`, for floating point values), the elementwise `Sum`, `Difference`, `Negation`, `Product` and `Division`, also with a scalar of the value type, the elementwise reciprocal `CwiseInverse`, `Transpose`, `Block`, the matrix product `InnerProduct`, `OuterProduct`, `CrossProduct`, `Norm` and `SquaredNorm`, which keep the value type, `Comparison` (`equal`, and `approxEqual`, which compares relatively, like Eigen's `isApprox()`), and `operator<<`.
* For `InvertibleValueMatrixBackend`, also provides the matrix `Inverse` of floating point matrices and the `Determinant` of floating point and integer matrices, which the transformations of covectors and of tensors with covariant indices need.

Evaluation and value semantics:

* A trait that computes a matrix, e.g. `Sum`, returns it from `value()`, of its type `type`, which may be a lazy expression of the library, like Eigen's.
  `Rebind` gives the matrix type of the backend with the value type of a matrix or an expression and a given size, e.g. for the scalar of an element of an unevaluated sum, and `Evaluate` evaluates a matrix or an expression into a new matrix of the type that `Rebind` gives for its value type and size.
  An expression must be move constructible, and that matrix constructible from it, also from a `const` reference: the core converts unevaluated values into evaluated ones with this constructor, e.g. an unevaluated sum into a vector.
  The operations of the core evaluate every result once, so they can be stored in `auto` variables (see "Results of operations are evaluated" in [DESIGN.md](DESIGN.md#results-of-operations-are-evaluated)).
* Values have value semantics with every backend: they never share their elements with another object.
  The core copies matrices only with `Copy`, which makes a deep copy, and moves and assigns them only with `Move`, which hands the elements over and may leave the source empty.
  So the matrices need a move constructor, but need not be copyable or assignable, like those of the archetype, and a library whose copies share the elements, like Blitz++, can still be adapted.
  The results of the other traits must not share the elements of their operands either: e.g. Blitz++'s transpose is a view of the original, so the Blitz++ backend copies it.
* The core accesses elements only with `Coefficient`, so a library with another element access than `matrix(row, col)` can be adapted.

### Units backends

A units backend:

* Defines its units as `wuzp::Unit<ImplementationT_>` of the units of its library, e.g. `wuzp::Unit<boost::units::si::length>`.
* Specializes `Product`, `Division`, `Power` (with rational exponents `wuzp::Rational<P, Q>`, e.g. for `pow<1, 2>()`), `Inverse` and `Identity` (the dimensionless unit) for its units.
  The core defines the sum and the difference of units, which only exist for equal units.
* Gives equal units the same type, however they are computed: the core compares units by their types, e.g. a length times a force must be the type of torque and energy.
  E.g. `wuzp_mp_units` normalizes the named units of mp-units, such as `si::newton`, to the units built from the base units.
* Specializes `AccessUnits<AccessImplementation<ValueT_>>` for every arithmetic value type with the units that the core and its aliases use, e.g. `Length` and `Force`, in which angles are dimensionless and torque is the unit of energy (see "Angles are dimensionless" in [DESIGN.md](DESIGN.md#angles-are-dimensionless)).
  A program uses exactly one units backend, since the specializations of `AccessUnits` of two backends would be the same.

### Checking a backend

* Check the concepts with a `static_assert` in the tests of the backend, e.g. `static_assert(wuzp::InvertibleValueMatrixBackend<my_backend::ValueMatrix>);` or `static_assert(wuzp::UnitSystem<wuzp::AccessUnits<wuzp::AccessImplementation<double>>>);`, like `tests/src/ValueMatrixBackends.cpp` and `tests/src/Unit.cpp`.
  Not in its headers: the check instantiates many expressions of the backend, which would cost compile time in every translation unit of its users.
  A trait that is not specialized at all fails with the message of its primary template, e.g. "'Norm' trait not specialized for given type.", and a trait with the wrong interface makes the concept unsatisfied, with notes of the compiler that name it.
* Run the test suite with it, which checks the semantics: add it to `WUZP_TEST_UNITS` or `WUZP_TEST_VALUES` in `tests/CMakeLists.txt`, to `tests/src/Unit.hpp` or `tests/src/ValueMatrix.hpp`, to the presets in `CMakePresets.json`, and to the CI matrix in `.github/workflows/ci.yml`, `ci/local.sh` and `ci/tidy.sh`.
  Tests that depend on a specific library, e.g. on lazy expressions, are limited to it with `#if VALUES_IMPLEMENTATION == ...`, compile tests with `// wuzp-requires: values=...` (see [Compile tests](#compile-tests)), and a comment that explains why.
* If a test fails with a backend that satisfies the concepts only because the core uses something the concepts do not promise, fix the core or extend the concept, not the backend: the archetypes must keep passing.

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

The CI workflow checks the format, runs clang-tidy, runs the tests for all backend combinations with GCC and Clang in both dev container variants (mp-units only in `latest`), for the archetypes in `latest`, and for one backend combination in release mode, runs them with AddressSanitizer and UndefinedBehaviorSanitizer, measures code coverage, tests the installed package and the use through `FetchContent`, and builds and runs the [benchmarks](benchmarks/README.md).
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
