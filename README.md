# ctm

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

## Test

```
sudo apt install libgtest-dev
cmake -DENABLE_TEST=ON ..
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

* Rotations
* Transformations
* Dynamic matrices
* Run-time frames
* Run-time units
* Unit tests

Improvements:

* Use TypeMatrix with rows or cols == 1 instead of std::tuple.

Minor:

* Downgrade C++ and CMake version as much as possible.

Documentation:  

* Examples
* Migration notes from kindr
