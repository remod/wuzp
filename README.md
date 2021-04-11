# ctm

## Build

```
mkdir build
cd build
cmake ..
make
```

## Format

Instead of `make`, run `make format`.

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
