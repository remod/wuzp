# Wrong Unit? Zero Points!

*wuzp* is a C++20 library for compile time analysis of units and coordinate systems for linear algebra.

[![CI](https://github.com/remod/wuzp/actions/workflows/ci.yml/badge.svg)](https://github.com/remod/wuzp/actions/workflows/ci.yml)
[![Code Coverage](https://codecov.io/gh/remod/wuzp/branch/master/graph/badge.svg?token=FVPNHZKXVX)](https://codecov.io/gh/remod/wuzp)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Introduction

Imagine a world where your IDE and compiler tell you when you are making a mistake like

```cpp
force = mass  // Error: Mismatching units!
force = mass * acceleration  // OK.
```

or

```cpp
velocity_in_frame_A + velocity_in_frame_B  // Error: Mismatching frames!
velocity_in_frame_A + transformation_B_to_A.transform(velocity_in_frame_B)  // OK.
```

or

```cpp
time_point + time_point  // Error: Cannot add two absolute quantities!
time_point + duration  // OK.
```

This speeds up development for two reasons:
First, you get faster feedback on errors in your code, and second, you do not have to write unit tests to cover these cases.

## Features

*wuzp* provides the following features:

* Compile time unit checks, including support for matrices with heterogeneous units (e.g. mass matrix).
* Compile time coordinate system checks, including transformations between coordinate systems.
* Distinction between absolute and relative units.[^1]

[^1]: Inspired by [Boost units](https://www.boost.org/doc/libs/1_61_0/doc/html/boost_units/Examples.html#boost_units.Examples.AbsoluteRelativeTemperatureExample), but extended to coordinate system transformations.

## Installation & Usage

*wuzp* integrates with the following unit and linear algebra libraries:

**Units:** 

* *wuzp*' own implementation
* [Boost](https://www.boost.org/doc/libs/1_61_0/doc/html/boost_units.html)

**Linear Algebra:**

* [Eigen](http://eigen.tuxfamily.org/)
* [Blitz++](https://github.com/blitzpp/blitz)

To install *wuzp* globally and update it together with your operating system, install it through the provided Debian packages.
This method will also implicitly install its 3rd-party dependencies.

```bash
sudo add-apt-repository ppa:remod/wuzp
sudo apt update
sudo apt install wuzp        # Includes wuzp's own unit implementation.
sudo apt install wuzp_boost  # When using Boost units.
sudo apt install wuzp_eigen  # When using Eigen.
sudo apt install wuzp_blitz  # When using Blitz++.
```

Alternatively, you can clone *wuzp* from GitHub and install its 3rd-party dependencies manually:

```bash
git clone https://github.com/remod/wuzp.git
sudo apt install libboost-dev   # When using Boost units.
sudo apt install libeigen3-dev  # When using Eigen.
sudo apt install libblitz0-dev  # When using Blitz++.
```

## Examples

For examples, have a look at the unit tests in the `tests` folder.

## Limitations

Do not use `auto` when working with Eigen (see [documentation](https://eigen.tuxfamily.org/dox/TopicPitfalls.html)).

Blitz++ cannot be installed with `libblitz0-dev` when compiled with Clang.
Instead, it needs to be cloned from GitHub and then built and installed manually (see [GitHub issue](https://github.com/blitzpp/blitz/issues/177)).

## Contributing

**Unit tests**

When adding a new feature, please make sure to add unit tests for it.
In order to run the tests locally, perform the following steps:

```bash
mkdir build
cd build
cmake -DENABLE_TESTS=ON -DENABLE_CODE_COVERAGE=ON ..
make -j
ctest
```

**Formatting**

The code is formatted using [clang-format](https://clang.llvm.org/docs/ClangFormat.html).

In order to format the code, perform the following steps:

```bash
sudo apt install clang-format
cmake -DENABLE_FORMAT=ON ..
make format
```

**Tidying**

The code is checked using [clang-tidy](https://clang.llvm.org/extra/clang-tidy/).

In order to check the code, perform the following steps:

```bash
sudo apt install clang-tidy
cmake -DENABLE_TIDY=ON ..
make tidy
```

**CI/CD**

The CI/CD workflow is configured to run the following steps:

* Check format
* Check tidy
* Check package versions (to enforce version bumps if necessary)
* Test source code
* Package
* Test packages
* Deliver packages

You can run an approximation of this workflow locally using the following script:

```bash
./scripts/run_workflow.sh
```
