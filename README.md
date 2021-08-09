# ctm

[![Build and Test](https://github.com/remod/ctm/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/remod/ctm/actions/workflows/build_and_test.yml)

## Preparations

```
sudo apt install git cmake libboost-dev libeigen3-dev
git clone https://github.com/remod/ctm.git
cd ctm
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

Features:

* Own units implementation
* Own matrix implementation
* Different rotations
* Non-static transformations
* Dynamic size matrices
* Run-time frames
* Run-time units
* Unit test coverage
* ROS/ROS2 release

Features (later):

* Tensor support

Minor:

* Downgrade C++ and CMake version as much as possible.

Documentation:  

* Examples
* Migration notes from kindr
