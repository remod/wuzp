# ctm

## Preparations

```
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
cmake -DENABLE_TEST=On ..
make
ctest ..
```

## Format

```
cmake -DENABLE_CLANG_FORMAT=On ..
make format
```

## Roadmap

Features:

* Rotations
* Transformations
* Unit tests

Improvements:

* Use TypeMatrix with rows or cols == 1 instead of std::tuple.
* Support Eigen lazy evaluation.

Documentation:  

* Examples
* Migration notes from kindr
