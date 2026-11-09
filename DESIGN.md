# Design

This document describes what *wuzp* models, the conventions it follows, the results of a review of its mathematical and physical soundness, and the design questions that are still open.
Open questions are marked with **TODO** and must be decided before 1.0.

## What wuzp models

*wuzp* adds compile-time checks to linear algebra libraries (Eigen, Blitz++):

* **Units.** Every element of a matrix has its own unit, so heterogeneous matrices (e.g. mass matrices with `kg` and `kg m^2` entries) are supported.
  Elementwise operations combine units elementwise, matrix products require all summands of an element to have the same unit.
  Units are either *wuzp*'s own implementation (seven SI exponents as reduced rationals), Boost.Units or mp-units.
* **Tensors.** `Tensor<Contravariant, Covariant, Quantities, CoordinateSystem, Absolute>` has up to rank 2.
  The number of indices is independent of the shape of the matrix storing the components.
  `innerProduct` contracts one index (the result has `c1 + c2 - 1` contravariant and `k1 + k2 - 1` covariant indices), `outerProduct` adds the indices.
* **Coordinate systems.** Every tensor is expressed in a coordinate system, a type without runtime cost.
  Operations combining tensors require them to be expressed in the same coordinate system.
  A coordinate system means axes and an origin; it is not a moving observer.
* **Absolute and relative tensors.** Absolute tensors are points, relative tensors are vectors (like temperatures and temperature differences).
* **Transformations.** Rotations, translations, proper rigid transformations (first rotation, then translation), Jacobians and Lorentz transformations, each in an active and a passive form.

## Conventions

**Transformation rules.**
For a linear transformation with component matrix `A` (contravariant components transform as `v' = A v`):

| Object | Indices | Rule | Implemented |
| --- | --- | --- | --- |
| Vector | (1,0) | `v' = A v` | yes |
| Point | (1,0), absolute | `p' = A p` (via a relative intermediate) | yes |
| Covector | (0,1) | `w' = w A^-1` | yes (Eigen only) |
| Linear map | (1,1) | `T' = A T A^-1` | yes (Eigen only) |
| Bilinear form, e.g. mass matrix | (0,2) | `M' = A^-T M A^-1` | yes (Eigen only) |
| E.g. covariance matrix | (2,0) | `T' = A T A^T` | yes |

`A^-1` appears twice in the rule for (0,2) tensors because `A` transforms components, so covariant indices transform with `A^-1`.
Equivalently, `A^-1` is the change-of-basis matrix `P` used by Wikipedia ("old = P new").
The notes at the end of this document start with the opposite convention (`M' = A^-1 M A`, `A^T M A`); the code follows the table above.

Translations move points and leave vectors unchanged.
A proper rigid transformation first rotates, then translates.
In the passive case, the rotation transforms from `From` into an intermediate coordinate system with the origin of `From` and the axes of `To`, and the translation from there into `To`; the translation is therefore expressed in `To`.
A translation must be expressed in the coordinate system of the objects it moves (active), or in `From` or `To` (passive), which have the same axes.
Active and passive transformations with the same parameters cancel each other.

`transformation.transform(otherTransformation)` re-expresses the other transformation in another coordinate system (conjugation); it does not compose them.

**Absolute and relative tensors.**
The rules follow affine geometry (and Boost.Units' absolute quantities):

| Operation | Result |
| --- | --- |
| point + vector, vector + point | point |
| point + point | error |
| point - vector | point |
| point - point | vector |
| vector - point | error |
| tensor * tensor, tensor / tensor, norm, cross product | only for vectors |

## Decisions

### Handling of Coordinate System Transformations

Options:

1. Handle coordinate system transformations on the level where the transformation objects operate.
   The coordinate system is assigned to the transformed object.
1. Handle coordinate system transformations deeper using a matrix of coordinate system transformation objects.
   A coordinate system transformation object has a "from" and a "to" coordinate system.
   Whenever a coordinate system transformation object is applied to an object with a "from" coordinate system, it turns into a "to" coordinate system.

Decision: 1.

Transforming certain tensors, e.g. a mass matrix, requires left and right multiplication with a transformation object.
This rules out option 2, as the two multiplications do not know about each other.
The information that the new coordinate system needs to be applied is only available on the level of the transformation object.

### Angles are dimensionless

The radian is a length divided by a length, so in SI angles are dimensionless.
Boost.Units treats the plane angle as a dimension of its own, and mp-units keeps the radian as a unit apart from one.
Following them would force explicit radians into ordinary formulas: `v + ω × r` only compiles after dividing `ω × r` by one radian, torque becomes `N m / rad`, and code written for one units backend does not compile with another.

Decision: angles are dimensionless in every units backend.
The Boost.Units and mp-units adapters define `Angle` as dimensionless, angular velocity as `1/s`, angular acceleration as `1/s^2` and torque as `N m`.

The price: `rad/s` cannot be told apart from `Hz`, nor torque from energy, so *wuzp* does not catch a missing factor of 2π between angular velocity and frequency.

### Constraints on template parameters

Template parameters used to be unconstrained: a test passed a `TypeMatrix` of coordinate systems as the coordinate system of a tensor by accident, and it compiled.

Decision: the template parameters are constrained with C++20 concepts (`wuzp/core/concepts.hpp`, #36), e.g. the coordinate system of a tensor must be a `CoordinateSystem`.
A parameter of the wrong kind fails with a constraint error, whose notes name the unsatisfied concept.
Invalid combinations of valid types, e.g. matrix sizes that do not match, are rejected with a `static_assert` and a clear message instead.

### Results of operations are evaluated

Eigen and Blitz++ return lazy expressions, which refer to their operands.
*wuzp* used to return them too, so whether a result referred to its operands depended on the operator: `auto x = makeVector() / 2.0;` was evaluated, while `auto x = makeVector() / quantity;` referred to a destroyed temporary (#69).
Some operations on such expressions did not compile, e.g. `-(a + b)` (#64).

Decision: the operators and member functions return evaluated values, quantities and tensors, which can be stored in `auto` variables.
The traits (e.g. `Sum<Lhs, Rhs>::type`) remain the lazy types: an operation computes the lazy expression of the backend and evaluates it once.
With Eigen, this costs no measurable time in the [benchmarks](benchmarks/README.md); in a chain such as `a + b + c`, each operation is evaluated on its own, though.
With Blitz++, every evaluation allocates an array.

Values, quantities and tensors have value semantics with every backend: a copy, a transpose or a block never shares the values of the original, although Blitz++'s copies, transposes and slices are views of the original (#60).
Blitz++ arrays cannot be moved, so a move hands the values over and leaves the moved-from array empty, like a moved-from dynamic Eigen matrix: a moved-from object can only be assigned to or destroyed.
Value matrix backends implement this with the traits `Copy` and `Move`, and their other traits must not return matrices that share the values of their operands (see `wuzp/core/traits.hpp`).

### Invalid conversions are explicit

Conversions between similar types, e.g. from an unevaluated sum into a vector, are implicit.
Those that fail their checks were implicit too, and failed with a `static_assert` in the body of the constructor, so `std::is_convertible` reported them as valid, and overloads such as `f(VectorInA)` and `f(VectorInB)` were ambiguous (#70).

Decision: they are `explicit` (`explicit(!IsValidConversion<...>::value)`).
Type traits report them as invalid, and copy initialization fails with the compiler's conversion error.
Direct initialization still reaches the constructor, whose `static_assert` explains what does not match, e.g. "Coordinate systems do not match.".
The specializations of `IsValidConversion` name each check, e.g. `CoordinateSystemsMatchV`, and the `static_assert`s use them, so that the two cannot drift apart.

### Integer values

Decision: operations whose result is not an integer, such as the norm or a power with a non-integer exponent, are computed in floating point and truncated to the value type, like the norm of integer vectors in Eigen: `pow<1, 2>()` of 5 m² is 2 m (#63).

## Review of mathematical and physical soundness (November 2026)

The library was reviewed for mathematical and physical soundness, with claims verified by compiling and running programs.

**Sound:**

* The unit exponent algebra and the canonical reduction of rational exponents.
* The unit inverse of matrices and its invertibility check, which accepts exactly the matrices with units of the form `u_i / v_j`, i.e. matrices that describe a linear map.
* The transformation rules for vectors, points, (1,1) and (0,2) tensors (`A^-T M A^-1` matches `R M R^T` to 1e-15 for a general rotation).
* Active and passive transformations cancel each other; the order of rotation and translation in rigid transformations; translations moving points but not vectors.
* The passive Lorentz boost matrix (standard form, `x^0 = c t` in length units, signature `(+,-,-,-)`, `Λ_0i = -γ β_i`), for the velocity of `To` relative to `From`, expressed in `From`, which must be smaller than the speed of light (#67).
* The rigid body kinematics in the tests (`v + ω × r`, `a + α × r + ω × (ω × r)` for a body-fixed point), and the kinetic energy and mass matrix of the rotating dumbbell.

**Defects** found by the review and while writing the compile tests, all fixed:

| Defect | Issue |
| --- | --- |
| `determinant()` returned the element unit instead of `u^N` | #47 |
| `TimePoint` and `Duration` used `AngularAcceleration` instead of `Time` | #48 |
| `TorqueVector` could not be instantiated | #49 |
| `getInvariant` returned a dangling Eigen expression (garbage at runtime) | #50 |
| The Lorentz boost matrix was NaN for zero velocity | #51 |
| Transforming covectors and (2,0) tensors compiled and returned `void` | #52 |
| Points and vectors converted implicitly into each other | #53 |
| Translations did not check coordinate systems | #54 |
| `Rational` `Power` specializations were wrong (`4^(1/4) = 1`); `ReduceRational` recursion was too deep | #55 |
| `Tensor::isApprox` did not check the covariant indices | - |
| `Tensor::hyperdeterminant` reported a confusing error instead of "Not implemented", and used the non-standard `typeof` | - |
| The checks "Quantities are not homogeneous." could never fail | - |
| `ActiveTranslation.hpp` and `PassiveTranslation.hpp` did not include what they use | - |
| `isApprox` was relative with Eigen, but absolute with Blitz++ | - |
| `zero()` and `random()` never compiled | - |

A code audit (November 2026) found these defects, all fixed:

| Defect | Issue |
| --- | --- |
| With Blitz++, copies shared the values of the original, so translations changed their `const` input | #60 |
| Dividing tensors did not check their coordinate systems | #61 |
| Converting values of another size was not checked | #62 |
| `pow<1, 2>()` of an integer quantity returned 1 | #63 |
| Some operations on unevaluated results did not compile | #64 |
| With mp-units, named units did not match the units *wuzp* computes | #65 |
| Copying a transformation from a non-const object did not compile | #66 |
| The Lorentz boost returned NaN for velocities of at least the speed of light | #67 |
| Whether a result referred to a destroyed temporary depended on the operator | #69 |
| Type traits reported invalid conversions as valid | #70 |

## Open design questions

### TODO: Products of tensors

`tensor * tensor` is the elementwise (Hadamard) product, also for rank 2 tensors: for a rotation `R` by 90°, `R * R` is `[0 1; 1 0]`, not `-I`.
The result claims to be a tensor in the same coordinate system, but it does not transform like one.
Users of Eigen expect `*` to be the matrix product.

Options: (a) `*` is the inner product (matrix product) for tensors, elementwise products get a named function; (b) no `*` between tensors at all, only named functions; (c) keep it.
Recommendation: (b) for 1.0, since it avoids silent misunderstandings in both directions; (a) can be added later.

The tensor product (`tensorProduct`) is not implemented in either backend, and its trait gives the result the index counts of the left operand instead of adding them.
Recommendation: remove it until it is implemented correctly.

### TODO: Meaning of `Tensor::inverse()`

`inverse()` computes the elementwise reciprocal but swaps the contravariant and covariant indices, which has no tensor meaning (the `inverse()` of a vector is a covector of reciprocals).
`tensorInverse()` is the matrix inverse.

Options: (a) `inverse()` is the matrix inverse of (1,1) tensors (mapping `T` to `T^-1`), and the elementwise reciprocal gets a different name; (b) keep both with clearer names.
Recommendation: (a).

### TODO: Operations on points

The rules for `+` and `-` are correct, but these operations on points compile although their result depends on the origin: `point * 2.0`, `point * quantity`, `point / quantity`, `-point`, `point.dotProduct(point)`.
`norm()`, the cross product and `tensor * tensor` correctly reject points.

Recommendation: reject all of them, with compile tests.
In the same spirit, `makeRelative()` of a relative tensor and `makeAbsolute()` of an absolute tensor are silently accepted, although the corresponding traits (`Absolute.hpp`) reject them; recommendation: reject them as well.

### TODO: Jacobians

A Jacobian is a `PassiveLinearTransformation`, so it also maps points: the Jacobian of polar to Cartesian coordinates at `r = 2` maps the point `(2 m, π/2)` to `(2, 3.14) m` instead of `(0, 2) m`.
A Jacobian is a local linearization and must only map tangent vectors.
The rule `A^-T M A^-1` cannot handle non-square Jacobians, and the mass matrix test is commented out (`tests/src/Jacobian.cpp:51`).
The physically meaningful operations are the pullback `M_q = J^T M_x J` (no inverse needed) and the operational space inertia `Λ = (J M^-1 J^T)^-1`.

Recommendation: a separate Jacobian type that rejects points and provides the pullback.

### TODO: Metric

`dotProduct` and `norm` use the Euclidean metric and contract two contravariant vectors.
This is fine in orthonormal coordinate systems, but not invariant under Jacobians or Lorentz transformations: the Euclidean "dot product" of a 4-vector changes from 1010100 to 1249730 m² under a boost, while the Minkowski invariant is preserved.
`SpacetimeVector` is just a 4D `PositionVector`.

Options: (a) a metric as part of the coordinate system, used by `dotProduct` and `norm`; (b) restrict `dotProduct` and `norm` to Euclidean coordinate systems and provide explicit metric contractions; (c) document the limitation.
Recommendation: (c) for 1.0, (a) later.

### TODO: Index order of mixed tensors

A (1,1) tensor does not record which of its indices is the upper and which the lower one (see the TODO in `Tensor.hpp`).
Recommendation: document that the first (row) index is contravariant and the second (column) index covariant, and keep it as a convention.

### TODO: Validity of rotations

Rotations are not checked for orthogonality or a determinant of 1, so a shear is accepted as a rotation.
The transformation rules use the general inverse and remain correct for any invertible matrix; only the guarantees of a rotation (e.g. preserving norms) break.
Recommendation: a runtime check in debug builds when constructing a rotation.

### TODO: Composition and inverse of transformations

There is no composition (`A→B` and `B→C` give `A→C`) and no inverse of transformations.
Recommendation: add both, type-checked through the coordinate systems; they are the most common operations in robotics.

### TODO: Moving coordinate systems

Coordinate systems are axes and an origin, not moving observers.
A passive transformation of a velocity re-expresses it; it does not add `ω × r` or the velocity of the coordinate system, and a rigid transformation of a torque ignores the moment arm (`t × F`).
The transport formulas exist only in the tests.
Recommendation: document this clearly; consider dedicated twist and wrench types later.

### TODO: Backend support for inverses

Blitz++ provides neither inverses nor determinants, so transformations of (0,1), (1,1) and (0,2) tensors only work with Eigen.
This weakens the goal that every feature works with every backend.
Recommendation: implement small fixed-size inverses for Blitz++ in *wuzp*, or document the restriction per backend.

### TODO: Absolute scalars

Absolute and relative only exist for tensors, so an absolute scalar (e.g. a time point or a temperature) needs a coordinate system, and there is no temperature alias.
The rigid transformation of a mass or inertia tensor does not compile, which is arguably safer than guessing the parallel axis term.
Recommendation: decide whether absolute quantities without a coordinate system are needed.

### TODO: Unreachable invertibility check

The check "Matrix is not invertible." (`Units.hpp`) can probably never fail: for matrices whose units do not describe a linear map, computing the candidate inverse already fails with "Units do not match.", and for all others, the check passes.
The unit of `determinant()` relies on the same computation to reject such matrices, and reports the same message (with Boost.Units, an error without a `static_assert`).
Recommendation: verify, then replace it with a check that runs first and reports a clear message, such as "The units of the matrix do not describe a linear map."

## Notes on tensor calculus

* [Tensor Operations: Contractions, Inner Products, Outer Products (Video)](https://www.youtube.com/watch?v=fDcfqfWRIi8&ab_channel=FacultyofKhan)
* [Matrix multiplication and tensorial summation convention](https://physics.stackexchange.com/questions/411015/matrix-multiplication-and-tensorial-summation-convention)
* [Converting between matrix multiplication and tensor contraction](https://physics.stackexchange.com/questions/252473/converting-between-matrix-multiplication-and-tensor-contraction)

The original notes, kept unchanged.
Note that the first block uses the opposite convention from the code (see [Conventions](#conventions)).

```text
// Tensor (3+) - Hypermatrix
// Tensor (2)  - Matrix
// Tensor (1)  - Vector
// Tensor (0)  - Scalar

// template <int ContravariantIndices, int CovariantIndices>
// class Tensor;
//
// using Scalar = Tensor<0, 0>;
// using EuclideanVector = Tensor<1, 0>;       // A * v
// using PassiveLinearTransformation = Tensor<1, 1>;  // M' = A^-1 * M * A
// using RotationMatrix = Tensor<1, 1>;        // M' = A^-1 * M * A
// using InertiaTensor = Tensor<0, 2>;         // M' = A^T * M * A

// n-m-Tensor:
// n = contravariant
// m = covariant

// 1-1-Tensor aka linear transformation in our convention:
// w = M * v
// w' = M' * v'
// w' = A * w = A * M * v = A * M * A^-1 * A * v = A * M * A^-1 * v'
// M' = A * M * A^-1

// 0-2-Tensor aka inertia tensor (aka mass matrix) in our convention:
// E = v^T * M * v
// E' = v^T' * M' * v'
// E' = E
// v^T * M * v = v^T * A^T * M' * A * v
// M = A^T * M' * A
// M' = (A^-1)^T * M * A^-1
// Our convention is confusing as we have two times A^-1, which intuitively corresponds to 2 contravariant indices.
// Probably this is due basis vectors being transformed actively while the measured quantities in these coordinate systems are transformed
// passively.

// 2-0-Tensor in our convention:
// Should be invariant?

// Einstein Summation

// A = A^i_j
// A^T = A_j^i

// https://en.wikipedia.org/wiki/Change_of_basis

// Note: Wiki uses the different convention "old = A * new", we use "new = A * old".

// e^i = A^i_j * e^j' = A * e'
// e^i' = (A^-1)^i_j * e^j = A^-1 * e
// e_i' = e_j * (A^-1)_j^i = e^T * (A^-1)^T

// https://en.wikipedia.org/wiki/Tensor

// Basis vector (row vector!!!)
// e_i' = e_j * A_i^j
// e' = e * A

// In our convention (column vector!!!), redefine f = e^T, f' = e^T', B = A^T:
// f^T' = f^T * B^T
// f' = B * f

// Contravariant vector (or simply "vector"):
// v^i, column vector
// v^i' = (A^-1)^i_j * v^j
// v' = A^-1 * v
// In our convention: v' = A * v

// Covariant:
// w_i, row vector
// w_i' = w_j * A_i^j
// w' = w * A
// In our convention: w' = w * A^-1

// Orthogonal matrix / transformation: A^-1 == A^T
```
