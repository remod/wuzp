# Changelog

All notable changes to *wuzp* are documented in this file.
The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and *wuzp* follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html) (see [CONTRIBUTING.md](CONTRIBUTING.md#versioning)).
Versions before 1.0 were distributed as Debian packages through a Launchpad PPA and are not listed here.

## [Unreleased]

### Changed

- **Breaking:** One CMake project and one installable package with the components `eigen`, `blitz` and `boost`, used as `find_package(wuzp CONFIG REQUIRED COMPONENTS eigen)` and `wuzp::wuzp`, `wuzp::eigen`, etc. It replaces the four separately packaged projects. *wuzp* can also be used through `FetchContent`.
- **Breaking:** The Debian packages and the Launchpad PPA are discontinued.
- **Breaking:** Points and vectors (absolute and relative tensors) no longer convert into each other implicitly. Use `makeRelative()` and `makeAbsolute()`.
- **Breaking:** Translations check coordinate systems: an active translation only moves points expressed in its coordinate system, and the vector of a passive translation from `From` to `To` is expressed in `From` or `To`. A passive proper rigid transformation declares its translation in `To`.
- **Breaking:** Powers of rationals that are not rationals themselves, such as `4^(1/4)`, are no longer defined (they returned wrong results), and `0^x` is only defined for `x > 0`.
- **Breaking:** `getInvariant(vector)` deduces its template arguments, and takes any vector whose components have the same unit, e.g. a four-velocity. Explicit template arguments no longer compile.
- The oldest supported dependency versions are those of Ubuntu 22.04: GCC 11, Clang 14, CMake 3.22, Eigen 3.4 and Boost 1.74.

### Added

- Support for Eigen 5, besides Eigen 3.4.
- Linear transformations transform covectors (`w' = w A^-1`) and tensors with two contravariant indices (`T' = A T A^T`). They returned `void` before.
- `zero()` and `random()` for values, quantities and tensors, with every backend. They did not compile before.
- Compile tests verify that invalid code does not compile, and that the error message explains why.
- The public headers compile on their own and without warnings with `-Wshadow -Wconversion -Wsign-conversion -Wold-style-cast`.

### Removed

- `getTypeName()`, which returned wrong names.

### Fixed

- The units of `TimePoint`, `Duration` and `TorqueVector`.
- Determinants have the product of the diagonal units.
- `Tensor::isApprox` checks the covariant indices.
- The homogeneity check of `Quantities`: elements with mixed units could be set with runtime indices.
- The Lorentz invariant read freed memory, and the Lorentz boost matrix was NaN for zero velocity.
- With Blitz++: `isApprox` compares relatively, like with Eigen; the dot product read a freed temporary; inner products keep the value type of the operands instead of widening it.
- Reducing rationals with large denominators exceeded the template instantiation depth.
- Translations did not compile with Clang 20 and newer.
- `Tensor::hyperdeterminant` failed with a confusing error instead of "Not implemented".

[Unreleased]: https://github.com/remod/wuzp/commits/master
