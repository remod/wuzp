# Changelog

All notable changes to *wuzp* are documented in this file.
The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and *wuzp* follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html) (see [CONTRIBUTING.md](CONTRIBUTING.md#versioning)).

## [Unreleased]

### Changed

- **Breaking:** One CMake project and one installable package with the components `eigen`, `blitz`, `boost` and `mp_units`, used as `find_package(wuzp CONFIG REQUIRED COMPONENTS eigen)` and `wuzp::wuzp`, `wuzp::eigen`, etc. It replaces the four separately packaged projects. *wuzp* can also be used through `FetchContent`.
- **Breaking:** Points and vectors (absolute and relative tensors) no longer convert into each other implicitly. Use `makeRelative()` and `makeAbsolute()`.
- **Breaking:** Translations check coordinate systems: an active translation only moves points expressed in its coordinate system, and the vector of a passive translation from `From` to `To` is expressed in `From` or `To`. A passive proper rigid transformation declares its translation in `To`.
- **Breaking:** Powers of rationals that are not rationals themselves, such as `4^(1/4)`, are no longer defined (they returned wrong results), and `0^x` is only defined for `x > 0`.
- **Breaking:** `getInvariant(vector)` deduces its template arguments, and takes any vector whose components have the same unit, e.g. a four-velocity. Explicit template arguments no longer compile.
- **Breaking:** With Boost.Units, angles are dimensionless, like with *wuzp*'s own units and mp-units: angular velocity is `1/s`, angular acceleration `1/s^2` and torque `N m`, so the same code compiles with every units backend (see "Angles are dimensionless" in `DESIGN.md`).
- **Breaking:** The template parameters of tensors, quantities, value matrices and transformations are constrained with C++20 concepts (`<wuzp/core/concepts.hpp>`), e.g. the coordinate system of a tensor must be a `CoordinateSystem`. Misuse fails with a constraint error, instead of compiling or failing deep inside the library.
- **Breaking:** Operators and member functions return evaluated values, quantities and tensors, which do not refer to their operands, so that results can be stored in `auto` variables, also when they are computed from temporaries. The traits, e.g. `Sum<Lhs, Rhs>::type`, are still the lazy types. Likewise, `getValues()` and `getQuantities()` of a temporary return its values rather than a reference into it.
- **Breaking:** Value matrix backends specialize the new traits `Copy` and `Move` (`wuzp/core/traits.hpp`), which copy and move their matrices without sharing the values, so that values, quantities and tensors have value semantics with every backend.
- **Breaking:** Conversions that fail their checks, e.g. of a vector in one coordinate system into another one, of a point into a vector, of a length into a time, or of values into values of another size, are explicit: `std::is_convertible` is false for them, and copy initialization fails with a conversion error. Direct initialization still fails with a message that explains why.
- **Breaking:** With mp-units, `wuzp_mp_units::UnitType` normalizes units to the coherent SI units built from the base units, e.g. `si::newton` to `kg m / s^2`, and rejects other units, e.g. kilometres or degrees, since *wuzp* does not convert units.
- **Breaking:** `createBoostMatrix` throws `std::invalid_argument` if the velocity is not smaller than the speed of light.
- Translations move a single point by adding the translation to it, instead of component by component, which is faster and also works for points with different units per component.
- The oldest supported dependency versions are those of Ubuntu 22.04: GCC 11, Clang 14, CMake 3.22, Eigen 3.4 and Boost 1.74.

### Added

- An [mp-units](https://mpusz.github.io/mp-units/) units backend, the `mp_units` component, for mp-units 2.5 or newer.
- Support for Eigen 5, besides Eigen 3.4.
- Support for macOS (Apple Clang) and, on a best-effort basis, Windows (MSVC), both tested in CI.
- `<wuzp/wuzp.hpp>` includes all headers of the core and the transformations.
- Benchmarks that compare *wuzp* with plain Eigen and Blitz++ at run time, and with plain Eigen at compile time (`benchmarks/README.md`).
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
- With Blitz++: `isApprox` compares relatively, like with Eigen; the dot product read a freed temporary; inner products and norms keep the value type of the operands instead of widening it, and inner products no longer copy an uninitialized member of Blitz++'s lazy reductions.
- Reducing rationals with large denominators exceeded the template instantiation depth.
- Translations did not compile with Clang 20 and newer.
- `Tensor::hyperdeterminant` failed with a confusing error instead of "Not implemented".
- With Blitz++, copies of values, quantities and tensors shared their values with the original, so translations changed their `const` input point, and transposes and blocks changed with the original. They have value semantics with every backend now; with Blitz++, a moved-from object is empty and can only be assigned to or destroyed ([#60](https://github.com/remod/wuzp/issues/60)).
- Dividing tensors did not check their coordinate systems ([#61](https://github.com/remod/wuzp/issues/61)).
- Converting values of another size, e.g. constructing a 3D vector from two values or calling `setValues()` with values of another size, was not checked, and read past the values with Blitz++. It fails with a message now ([#62](https://github.com/remod/wuzp/issues/62)).
- `pow<1, 2>()` of an integer quantity returned 1: non-integer exponents are computed in floating point, and the result is converted to the value type, which truncates like the norm of integer vectors ([#63](https://github.com/remod/wuzp/issues/63)).
- Operations on the results of operations did not compile: `-(a + b)`, `sin(a + b)`, `cos(a + b)`, `translation.transform(p + v)`, and with Blitz++ the dot product, transpose and blocks of expressions ([#64](https://github.com/remod/wuzp/issues/64)).
- With mp-units, named units such as `si::newton`, `si::joule` and `si::hertz` did not match the units *wuzp* computes ([#65](https://github.com/remod/wuzp/issues/65)).
- Copying a transformation, a tensor, quantities or values from a non-const object used the forwarding constructor, which did not compile for transformations ([#66](https://github.com/remod/wuzp/issues/66)).
- The Lorentz boost returned NaN for velocities of at least the speed of light. It is documented that the velocity is the one of `To` relative to `From`, expressed in `From` ([#67](https://github.com/remod/wuzp/issues/67)).
- Whether a result referred to its operands depended on the operator: `auto x = makeVector() / QuantityD<Dimensionless>(2.0);` read a destroyed temporary, `makeVector() / 2.0` did not ([#69](https://github.com/remod/wuzp/issues/69)).
- Type traits reported invalid conversions as valid, e.g. `std::is_convertible` of a vector in one coordinate system into another one, which made overloads ambiguous ([#70](https://github.com/remod/wuzp/issues/70)).

[Unreleased]: https://github.com/remod/wuzp/commits/master
