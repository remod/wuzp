# Wrong Unit? Zero Points!

*WUZP* is a C++ library for compile time analysis of units and coordinate frames for linear algebra.

[![Build and Test](https://github.com/remod/wuzp/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/remod/wuzp/actions/workflows/build_and_test.yml)
[![Code Coverage](https://codecov.io/gh/remod/wuzp/branch/master/graph/badge.svg?token=FVPNHZKXVX)](https://codecov.io/gh/remod/wuzp)

## Introduction

When writing C++ code, it is often desirable to discover as many errors as possible already in the compilation stage.
This helps to fix these errors early in the development phase, reduces the number of unit tests which have to be written or runtime sanity checks to be performed.

[Eigen](https://gitlab.com/libeigen/eigen) is a popular library to write linear algebra code.
While it is extandable to use different scalar types, it is not possible to introduce physical quantities like the ones provided by libraries like [boost/units](https://github.com/boostorg/units) or [nholthaus/units](https://github.com/nholthaus/units) (TODO: source).

This is where *WUZP* comes into play.

## Preparations

```
sudo apt install git cmake libboost-dev libeigen3-dev
git clone https://github.com/remod/wuzp.git
cd wuzp
mkdir build
cd build
```

## Build

```
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

## Tests

```
sudo apt install libgtest-dev
cmake -DENABLE_TESTS=ON ..
make
ctest ..
```

## Format

```
sudo apt install clang-format
cmake -DENABLE_FORMAT=ON ..
make format
```

## Tidy

```
sudo apt install clang-tidy
cmake -DENABLE_TIDY=ON ..
make tidy
```

## Roadmap

**Features**

Value matrix:

* Own matrix implementation
* Dynamic sized matrices

Units:

* Run-time units
* Support nholthaus/units

Rotations:

* Different rotations (quaternions, angle axis, etc.)

Transformations:

* Run-time frames

Pipeline & packaging:

* Migrate from macros to different submodules
* Unit test coverage
* ROS/ROS2 release
* Downgrade C++ and CMake version as much as possible

Documentation:  

* Examples
* Limitations
  * In-place operations not supported if they change the type
* Migration notes from kindr

## Design Decisions

## Order of methods

1. Empty constructor
1. Copy constructor
1. Move constructor
1. Constructor copying members
1. Constructor moving members
1. Destructor
1. Others

### Handling of Coordinate System Transformations

Options:

1. Handle coordinate system transformations on the level where the transformation objects operate.
   The coordinate frame is assigned to the transformed object.
1. Handle coordinate system transformations deeper using a matrix of coordinate system transformation objects.
   A coordinate system transformation object has a "from" and a "to" coordinate system.
   Whenever a coordinate system transformation object is applied to an object with a "from" coordinate system, it turns into a "to" coordinate system.

Decision: 1.

Transforming certain tensors, e.g. a mass matrix, requires left and right multiplication with a transformation object.
This rules out option 2, as the two multiplications do not know about each other.
The information that the new coordinate system needs to be applied is only available on the level of the transformation object.
