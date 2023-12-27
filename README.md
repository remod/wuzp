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

It prevents errors like mixing units when operating on quantities:

```cpp
force = mass  // Error: Mismatching units!
force = mass * acceleration  // OK.
```

While relative quantities can be operated on freely, there are some restrictions for absolute quantities.[^1]
For example, it does not make sense to add two absolute quantities:

```cpp
time_point + time_point  // Error: Cannot add two absolute quantities!
time_point + duration  // OK.
```

When operating with different coordinate systems, it is crucial to prevent mixing them up:

```cpp
velocity_in_A + velocity_in_B  // Error: Mismatching coordinate systems!
velocity_in_A + transformation_B_to_A.transform(velocity_in_B)  // OK.
```

**Note:** While a single object can only have a single coordinate system, it is possible to assign it a heterogeneous set of units.
This is required e.g. to represent mass matrices, which contain `kg` for translational degrees of freedom and `kg m^2` for rotational degrees of freedom.

## Transformations

In general, a transformation can be one or a combination of the following unit transformations:

* Rotation
* Translation
* Reflection
* Dilation

*wuzp* supports the following transformations as of today:

* Rotation
* Translation
* Proper rigid transformation (first rotation, then translation)

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

Tensors may only be 0-, 1-, or 2-dimensional.

Do not use the keyword `auto` when using the Eigen implementation (see [documentation](https://eigen.tuxfamily.org/dox/TopicPitfalls.html)).

Blitz++ cannot be installed with `libblitz0-dev` when compiled with Clang, because the Debian package does not contain a required header file.
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

In order to format the code, navigate to the `build` directory and perform the following steps:

```bash
sudo apt install clang-format
cmake -DENABLE_FORMAT=ON ..
make format
```

**Tidying**

The code is tidied using [clang-tidy](https://clang.llvm.org/extra/clang-tidy/).

In order to tidy the code, navigate to the `build` directory and perform the following steps:

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


[^1]: Inspired by [Boost units](https://www.boost.org/doc/libs/1_61_0/doc/html/boost_units/Examples.html#boost_units.Examples.AbsoluteRelativeTemperatureExample).

[^2]: The difference between active and passive transformations is explained [here](https://en.wikipedia.org/wiki/Active_and_passive_transformation).