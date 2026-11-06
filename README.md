# Wrong Unit? Zero Points!

*wuzp* is a C++20 library which extends existing numeric algebra libraries with

* unit type safety,
* coordinate system type safety, and
* transformations.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![CI](https://github.com/remod/wuzp/actions/workflows/ci.yml/badge.svg)](https://github.com/remod/wuzp/actions/workflows/ci.yml)
[![Code Coverage](https://codecov.io/gh/remod/wuzp/branch/master/graph/badge.svg?token=FVPNHZKXVX)](https://codecov.io/gh/remod/wuzp)

## Type Safety

Type safety speeds up development because your IDE, your compiler will report any mistakes without you having to write and run unit tests.

*wuzp* provides compile time type safety for units and coordinate systems.

It prevents errors like mixing quantities with incompatible units ([code example](examples/quantity.cpp)):

```cpp
force = mass  // Error: Mismatching units!
force = mass * acceleration  // OK.
```

While relative quantities can be operated on freely, there are some restrictions for absolute quantities.[^1]
For example, it does not make sense to add two absolute quantities ([code example](examples/absolute_quantity.cpp)):

```cpp
time_point + time_point  // Error: Cannot add two absolute quantities!
time_point + duration  // OK.
```

When operating with different coordinate systems, it is crucial to prevent mixing them up:

```cpp
force_in_A + force_in_B  // Error: Mismatching coordinate systems!
force_in_A + transformation_B_to_A.transform(force_in_B)  // OK.
```

**Note:** While a single object can only have a single coordinate system, it is possible to assign it a heterogeneous set of units.
This is required e.g. to represent mass matrices, which contain `kg` for translational degrees of freedom and `kg m^2` for rotational degrees of freedom.

## Transformations

In general, a transformation can be one or a combination of the following base transformations:

* Rotation
* Translation
* Reflection
* Dilation

*wuzp* supports the following transformations as of today:

* Rotation
* Translation
* Proper rigid transformation (= first rotation, then translation)
* Jacobian
* Lorentz transformation

These transformations are supported in their **active** and **passive** form.
In the **active** case the object is transformed, resulting in a new object.
In the **passive** case there is only a single object, but the coordinate system which it is expressed in is transformed.[^2]

As mentioned above, *wuzp* distinguishes between absolute and relative quantities.
In geometric terms, absolute positions can be interpreted as points, while relative positions can be interpreted as vectors.
This distinction becomes important when transforming points and vectors:
Unlike points, vectors are agnostic to their position and therefore unaffectd by translations.

The following table visualizes how a proper rigid transformation transforms a point and a vector in the active and passive case:

| | Target: Point | Target: Vector |
| --- | --- | --- |
| Type:<br>Active | <img src="doc/active_proper_rigid_transformation_of_point.svg" alt="Active Proper Rigid Transformation of Point" height="200"/><br>P is first rotated, then translated into P'. | <img src="doc/active_proper_rigid_transformation_of_vector.svg" alt="Active Proper Rigid Transformation of Vector" height="200"/><br>*r* is first rotated, then translated into *r'*. As a vector is agnostic to its position, only the rotational part of the transformation affects its components. |
| Type:<br>Passive | <img src="doc/passive_proper_rigid_transformation_of_point.svg" alt="Passive Proper Rigid Transformation of Point" height="200"/><br>P is expressed in coordinate system A, then in coordinate system B. | <img src="doc/passive_proper_rigid_transformation_of_vector.svg" alt="Active Proper Rigid Transformation of Vector" height="200"/><br>*r* is expressed in coordinate system A, then in coordinate system B. As a vector is agnostic to its position, only the rotational part of the transformation affects its components. |

## Installation & Usage

*wuzp* integrates with the following unit and linear algebra libraries:

**Units:** 

* *wuzp*' own implementation
* [Boost](https://www.boost.org/doc/libs/1_61_0/doc/html/boost_units.html)

**Linear Algebra:**

* [Eigen](http://eigen.tuxfamily.org/)
* [Blitz++](https://github.com/blitzpp/blitz)

*wuzp* is header-only and requires a C++20 compiler and CMake 3.22 or newer.
Each backend is an optional component with its own dependency:

| Component | Target | Dependency (minimum version) |
| --- | --- | --- |
| core, including *wuzp*' own units | `wuzp::wuzp` | none |
| `eigen` | `wuzp::eigen` | Eigen 3.4 |
| `blitz` | `wuzp::blitz` | Blitz++ 1.0.2 |
| `boost` | `wuzp::boost` | Boost 1.74 |

**Using CMake's FetchContent:**

```cmake
include(FetchContent)
FetchContent_Declare(wuzp GIT_REPOSITORY https://github.com/remod/wuzp.git GIT_TAG master)
FetchContent_MakeAvailable(wuzp)
target_link_libraries(my_target PRIVATE wuzp::wuzp wuzp::eigen)
```

**Installing and using `find_package`:**

```bash
git clone https://github.com/remod/wuzp.git
cmake -S wuzp -B wuzp/build
cmake --install wuzp/build  # Installs the components whose dependencies were found.
```

```cmake
find_package(wuzp CONFIG REQUIRED COMPONENTS eigen)
target_link_libraries(my_target PRIVATE wuzp::wuzp wuzp::eigen)
```

By default, a backend is enabled if its dependency is found.
Use `-DWUZP_WITH_EIGEN=ON|OFF` (and likewise `BLITZ`, `BOOST`) to require or disable one.

## Examples

Examples demonstrating how to use *wuzp* can be found in the `examples` folder.
They are built together with the project if the Eigen backend is enabled, e.g.:

```bash
cmake --preset gcc-wuzp-eigen
cmake --build --preset gcc-wuzp-eigen
./build/gcc-wuzp-eigen/examples/example_transformation
```

For more example code, have a look at the unit tests in the `tests` folder.

## Limitations

Tensors may only be 0-, 1-, or 2-dimensional.

Do not use the keyword `auto` when using the Eigen implementation (see [documentation](https://eigen.tuxfamily.org/dox/TopicPitfalls.html)).

Blitz++ cannot be installed with `libblitz0-dev` when compiled with Clang, because the Debian package does not contain a required header file.
Instead, it needs to be cloned from GitHub and then built and installed manually (see [GitHub issue](https://github.com/blitzpp/blitz/issues/177)).

## Contributing

**Development environment**

Development happens in a [dev container](https://containers.dev/), the same container image that CI uses, so local builds and CI see the same compilers, libraries and tools.
There are two variants:

* `min`: Ubuntu 22.04 with the oldest supported dependency versions (GCC 11, Clang 14, CMake 3.22, Eigen 3.4, Boost 1.74).
* `latest`: Ubuntu 26.04 with the newest supported dependency versions (GCC 15, Clang 21, CMake 4.2, Boost 1.90).

Open the repository in VS Code or CLion and choose *Reopen in Container*, or use the [Dev Container CLI](https://github.com/devcontainers/cli):

```bash
devcontainer up --workspace-folder . --config .devcontainer/latest/devcontainer.json
devcontainer exec --workspace-folder . --config .devcontainer/latest/devcontainer.json bash
```

The images are published by CI.
If the Dockerfile was changed locally, the image is built locally instead.

**Building and testing**

The test suite runs against every combination of units backend and value matrix backend, to make sure *wuzp* does not depend on one particular implementation.
Each combination has a CMake preset named `<compiler>-<units>-<values>`, e.g. `gcc-wuzp-eigen` or `clang-boost-blitz` (see `cmake --list-presets`):

```bash
cmake --preset gcc-wuzp-eigen
cmake --build --preset gcc-wuzp-eigen
ctest --preset gcc-wuzp-eigen
```

When adding a new feature, please make sure to add unit tests for it, including tests verifying that invalid code does not compile.

**Compile tests**

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

**Formatting and tidying**

The code is formatted using [clang-format](https://clang.llvm.org/docs/ClangFormat.html), with the version pinned in the dev container, and checked with [clang-tidy](https://clang.llvm.org/extra/clang-tidy/):

```bash
cmake --build --preset gcc-wuzp-eigen --target wuzp_format
cmake --build --preset gcc-wuzp-eigen --target wuzp_tidy
```

To format the code automatically before each commit, enable the [pre-commit](https://pre-commit.com/) hook with `pre-commit install`.

**Continuous integration**

The CI workflow checks the format, runs the tests for all backend combinations with GCC and Clang in both dev container variants, runs them with AddressSanitizer and UndefinedBehaviorSanitizer, measures code coverage, and tests the installed package and the use through `FetchContent`.
Warnings are errors in CI (`WUZP_WARNINGS_AS_ERRORS`, on in all presets), and every public header is compiled on its own with strict warnings.
Each job is a script in `ci/`, and `ci/local.sh` runs the same jobs locally in the same containers (requires Docker):

```bash
ci/local.sh                 # All jobs.
ci/local.sh format test-min # Selected jobs: format, test-min, test-latest, coverage, sanitizers, install.
```

By default, `ci/local.sh` uses half of the CPU cores; set `WUZP_JOBS` to change this.

Codecov checks that the coverage does not drop and that new code is covered (`codecov.yml`).
Pushing a tag `v<version>` that matches the version in `CMakeLists.txt` creates a GitHub release.

[^1]: Inspired by [Boost units](https://www.boost.org/doc/libs/1_61_0/doc/html/boost_units/Examples.html#boost_units.Examples.AbsoluteRelativeTemperatureExample).

[^2]: The difference between active and passive transformations is explained [here](https://en.wikipedia.org/wiki/Active_and_passive_transformation).