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
- **Breaking:** `QuantityI`, `QuantityF` and `QuantityD` are defined by the value matrix backends, e.g. `wuzp_eigen::QuantityD<Length>` instead of `wuzp::QuantityD<Length>`, since they need a value matrix backend. `<wuzp/core/ValueWrapper.hpp>` and `wuzp::ValueWrapper<ValueT>` are removed: use the backend's `Value<ValueT>`, e.g. `wuzp_eigen::Value<double>`. `getSpeedOfLight` takes the value matrix backend: `getSpeedOfLight<wuzp_eigen::ValueMatrix, double>()`.
- **Breaking:** Value matrix backends specialize the traits only for their own matrices and expressions, e.g. `template <EigenArray ImplementationT_>`, and specialize the new traits `Coefficient` (element access) and `Rebind` (values of another value type and size of the same backend). Operations derive scalars from the backend of their operands. See "The backend interface" in `DESIGN.md`.
- **Breaking:** The template parameters that take a value matrix backend, e.g. of `HomogeneousVector`, `PositionVector3` or `ActiveLinearTransformation`, must be the `ValueMatrix` of a value matrix backend (`AnyValueMatrix`), e.g. `wuzp_eigen::ValueMatrix`, not e.g. `Eigen::Array`.
- **Breaking:** With Eigen, evaluated values have Eigen's default storage order, so that results have one type: e.g. the transpose of a `ValueMatrix<double, 3, 2>` is a `ValueMatrix<double, 2, 3>`, rather than a row-major array.
- **Breaking:** Tensors record the variance of each index, in their order: `Tensor<Indices<Up, Down>, Quantities, CoordinateSystem, Absolute>` replaces `Tensor<1, 1, Quantities, CoordinateSystem, Absolute>`, e.g. `Indices<Down, Down>` for a mass matrix. The aliases, e.g. `Vector`, `Covector`, `Point` and `PositionVector3`, are unchanged. The components are stored in a canonical shape: a 1x1 matrix for rank 0, a column for a vector, or several columns for a batch of vectors or points, e.g. a point cloud, a row for a covector, and a square matrix for rank 2 (see "Index variance and order" in `DESIGN.md`). The operations that are only defined for a single vector, e.g. `innerProduct`, `norm()` and `transpose()`, reject batches.
- **Breaking:** `transpose()` swaps the indices: the transpose of a vector is a covector, and that of `T^i_j` is `T_j^i`. `innerProduct` contracts the last index of the left-hand side with the first index of the right-hand side, which must be an upper and a lower index, whatever the shape of the operands: e.g. `innerProduct(v, w)` of a vector and a covector is the scalar `v^i w_i`, and `innerProduct(v.transpose(), innerProduct(M, v))`, which contracts two lower indices, becomes `innerProduct(v, innerProduct(M, v))`. `outerProduct` concatenates the indices of two vectors or covectors, e.g. `outerProduct(v, v)` has two upper indices.
- **Breaking:** `*` and `/` of tensors only take scalars: arithmetic values, values and quantities of size 1x1, and tensors of rank 0. The elementwise products and quotients of tensors of rank 1 or more, and of tensors with quantities or values of other sizes, are rejected: they depend on the basis, so `cwiseProduct` and `cwiseQuotient` compute them as quantities (see "Products of tensors" in `DESIGN.md`).
- **Breaking:** Linear transformations, e.g. rotations, Jacobians and Lorentz transformations, store their matrix as quantities, returned by `getQuantities()`, and are constructed from values or quantities, e.g. `Rotation{tensor.getQuantities()}` rather than `Rotation{tensor}`. `getTensor()` returns the matrix as a linear map, and only exists for square matrices: the matrix of a Jacobian maps one coordinate system into another, possibly of another dimension. Translations provide `getQuantities()` too, and `isApprox` of transformations compares their quantities.
- **Breaking:** Points follow the rules of affine spaces: `p - q` is a vector, `p + v`, `v + p` and `p - v` are points, and the operations whose result depends on the origin of the coordinate system are rejected, e.g. `p * 2.0`, `p * quantity`, `p / quantity`, `-p`, `p.dotProduct(q)`, inner products with a point and its inverse. Affine combinations are written `p + (q - p) * t`. `makeRelative()` of a vector and `makeAbsolute()` of a point are rejected; both are relative to the origin of the coordinate system (see "Points and vectors" in `DESIGN.md`).
- **Breaking:** Only tensors with one upper index, vectors and batches of vectors, can be absolute: e.g. absolute covectors and absolute scalars are rejected. An absolute scalar, e.g. a time point, is a point in a one-dimensional coordinate system, like `TimePoint`. Linear transformations only transform relative covectors.
- **Breaking:** `inverse()` is the matrix inverse of values, quantities and tensors, like Eigen's `MatrixBase::inverse()`. The inverse of a tensor exists for rank 0 and 2, and maps the indices `(X, Y)` to `(not Y, not X)`, e.g. the inverse of a metric `g_ij` is `g^ij`; vectors and covectors have none. The elementwise reciprocal is `cwiseInverse()`, which returns quantities for tensors, like `cwiseProduct`, and also works with Blitz++. Value matrix backends specialize the trait `Inverse` for the matrix inverse and the new trait `CwiseInverse` for the elementwise reciprocal (see "Inverses and determinants" in `DESIGN.md`).
- **Breaking:** `determinant()` of a tensor is only defined for tensors of rank 0 and for the mixed tensors of rank 2, e.g. a linear map, whose determinant is invariant. The determinant of a tensor with two upper or two lower indices depends on the basis: `getQuantities().determinant()` computes it. The determinant of quantities is also defined for 1x1 matrices.
- **Breaking:** The matrix inverse and the determinant reject units that do not describe a linear map, `M_ij = a_i / b_j`, with a clear message, also with Boost.Units, which failed with an error without a message before.
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
- Concepts for the backend interface, `ValueMatrixBackend`, `InvertibleValueMatrixBackend` and `UnitSystem` (`<wuzp/core/concepts.hpp>`). The tests check every backend with them, and authors of other backends can do the same, so that a backend that misses a trait fails in its tests, rather than deep inside the core.
- Several value matrix backends can be used in one program, also in one translation unit. A program uses exactly one units backend.
- Compile tests verify that invalid code does not compile, and that the error message explains why.
- The public headers compile on their own and without warnings with `-Wshadow -Wconversion -Wsign-conversion -Wold-style-cast`.
- `cwiseProduct` and `cwiseQuotient` of tensors, which return the elementwise product and quotient of their components as quantities, and the elementwise `division` of quantities of the same size.
- Products of tensors of rank 0 with tensors of any rank, e.g. of a scalar field and a vector.
- Quantities and tensors can be multiplied and divided by arithmetic scalars of another type, if their common type is the value type, e.g. `2 * v` for a vector of `double`.
- The trait `UnitsDescribeLinearMap`, which checks that the units of a matrix describe a linear map, `M_ij = a_i / b_j`.

### Removed

- `getTypeName()`, which returned wrong names.
- The trait `AccessValues`, the global default value matrix backend, which `Rebind` replaces.
- `tensorProduct` and the trait `TensorProduct`, which no backend implemented and which gave the result the indices of the left operand: `outerProduct` is the tensor product.
- `IsSpecializationInts`, which `IsSpecialization` replaces now that the template parameters of `Tensor` are types.
- `tensorInverse()` and the trait `TensorInverse`: `inverse()` is the matrix inverse.
- The aliases `Copoint` and `HomogeneousCopoint`: covectors cannot be absolute.
- The unused specialization of `Identity` for Boost.Units' absolute units.

### Fixed

- The units of `TimePoint`, `Duration` and `TorqueVector`.
- Determinants have the product of the diagonal units.
- `Tensor::isApprox` checks the covariant indices.
- The homogeneity check of `Quantities`: elements with mixed units could be set with runtime indices.
- The Lorentz invariant read freed memory, and the Lorentz boost matrix was NaN for zero velocity.
- With Blitz++: `isApprox` compares relatively, like with Eigen; the dot product read a freed temporary; inner products and norms keep the value type of the operands instead of widening it, and inner products no longer copy an uninitialized member of Blitz++'s lazy reductions.
- Reducing rationals with large denominators exceeded the template instantiation depth.
- Including both value matrix backends in one translation unit was a redefinition error, and using them in different translation units of one program violated the one definition rule.
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
- The index rules of `innerProduct` were unsound: the inner product of a vector and a covector was a 3x3 matrix typed as a scalar, and inner products contracted two upper indices ([#71](https://github.com/remod/wuzp/issues/71)). `outerProduct(v, v)` of two vectors failed with "Hypermatrices are not supported".

[Unreleased]: https://github.com/remod/wuzp/commits/master
