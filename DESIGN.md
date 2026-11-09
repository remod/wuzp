# Design

This document describes what *wuzp* models, the conventions it follows, the results of a review of its mathematical and physical soundness, and the design questions that are still open.
Open questions are marked with **TODO** and must be decided before 1.0.

## What wuzp models

*wuzp* adds compile-time checks to linear algebra libraries (Eigen, Blitz++):

* **Units.** Every element of a matrix has its own unit, so heterogeneous matrices (e.g. mass matrices with `kg` and `kg m^2` entries) are supported.
  Elementwise operations combine units elementwise, matrix products require all summands of an element to have the same unit.
  Units are either *wuzp*'s own implementation (seven SI exponents as reduced rationals), Boost.Units or mp-units, one of them per program (see [The backend interface](#the-backend-interface)).
* **Tensors.** `Tensor<Indices, Quantities, CoordinateSystem, Absolute>` has up to rank 2.
  `Indices` records the variance of each index in its order, e.g. `Indices<Up, Down>` for a linear map `T^i_j`, and the indices determine the shape of the matrix storing the components (see [Index variance and order](#index-variance-and-order)).
  `innerProduct` contracts the last index of the left-hand side with the first index of the right-hand side, `outerProduct` concatenates the indices, and `*` and `/` only take scalars (see [Products of tensors](#products-of-tensors)).
* **Coordinate systems.** Every tensor is expressed in a coordinate system, a type without runtime cost.
  Operations combining tensors require them to be expressed in the same coordinate system.
  A coordinate system means axes and an origin; it is not a moving observer.
* **Points and vectors.** Absolute tensors are points, relative tensors are vectors and all other tensors (like temperatures and temperature differences).
  Points follow the rules of affine spaces: operations whose result depends on the origin of the coordinate system are rejected (see [Points and vectors](#points-and-vectors)).
* **Transformations.** Rotations, translations, proper rigid transformations (first rotation, then translation), Jacobians and Lorentz transformations, each in an active and a passive form.
  They compose with `*`, in the order of Drake's monogram notation, and have an `inverse()` (see [Composition and inverse of transformations](#composition-and-inverse-of-transformations)).

## Conventions

**Transformation rules.**
For a linear transformation with component matrix `A` (contravariant components transform as `v' = A v`):

| Object | Indices | Rule | Implemented |
| --- | --- | --- | --- |
| Vector | `Indices<Up>` | `v' = A v` | yes |
| Point | `Indices<Up>`, absolute | `p' = A p` (via a relative intermediate) | yes |
| Covector | `Indices<Down>` | `w' = w A^-1` | yes (Eigen only) |
| Linear map | `Indices<Up, Down>` | `T' = A T A^-1` | yes (Eigen only) |
| Bilinear form, e.g. mass matrix | `Indices<Down, Down>` | `M' = A^-T M A^-1` | yes (Eigen only) |
| E.g. covariance matrix | `Indices<Up, Up>` | `T' = A T A^T` | yes |

`A^-1` appears twice in the rule for bilinear forms because `A` transforms components, so covariant indices transform with `A^-1`.
Equivalently, `A^-1` is the change-of-basis matrix `P` used by Wikipedia ("old = P new").
The notes at the end of this document start with the opposite convention (`M' = A^-1 M A`, `A^T M A`); the code follows the table above.

Translations move points and leave vectors unchanged.
A proper rigid transformation first rotates, then translates: `x -> R x + t`.
An active transformation moves the objects expressed in its coordinate system.
A passive transformation from `From` to `To` expresses the objects of `From` in `To`, and only takes objects expressed in `From`.
`PassiveTransformation<From = B, To = A>` is `X_AB` in the [monogram notation of Drake](https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html) and `^A_B T` in the notation of Craig (Introduction to Robotics, chapter 2): it maps the coordinates of a point in `B` to its coordinates in `A`.
The vector of a passive translation is the vector from the origin of `To` to the origin of `From`, expressed in `To`, i.e. Drake's `p_AoBo_A`; `From` and `To` have the same axes, so it has the same components in `From`.
In a passive proper rigid transformation, the rotation transforms from `From` into an intermediate coordinate system with the origin of `From` and the axes of `To`, and the translation from there into `To`; the translation is therefore expressed in `To`, like that of Drake's [RigidTransform](https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rigid_transform.html) `X_AB = (R_AB, p_AoBo_A)`.
The matrix `A` of a passive linear transformation maps the components of a vector in `From` to its components in `To`: its rows are the dual basis covectors of `To`, and the basis vectors of `To` are the columns of `A^-1`, all expressed in `From` (Carroll, eqs. 1.38 and 1.30); only for orthonormal bases, e.g. for a rotation, are the rows of `A` also the basis vectors of `To`.

An active transformation and the passive transformation with the same parameters cancel each other: the passive transformation from `From` to `To` with the parameters of a motion is the one to the coordinate system `To` that the motion moves `From` onto, and it is the inverse of the active transformation by that motion.
Its matrix and vector are those of the inverse:

* The active rotation by `R` and the passive rotation by the same angle around the same axis, whose matrix is `R^T`.
* The active translation by `t` and the passive translation by `t`, whose vector, from the origin of `To` to the origin of `From`, is `-t`.
* The active proper rigid transformation `(R, t)` and the passive one with the same rotation and translation, `(R^T, -R^T t)`, whose translation `-R^T t` is expressed in `To`.
  It is not `-t`, unless `R t = t`: the passive transformation `(R^T, -t)` maps the point `(1, 2, 3)`, moved by the rotation of `pi/2` around `x` and the translation `(0, 0, 1)`, to `(1, 3, 2)` (`tests/src/ActiveAndPassiveProperRigidTransformation.cpp`).

Transformations compose with `*` and have an `inverse()` (see [Composition and inverse of transformations](#composition-and-inverse-of-transformations)).
`activeTransformation.transform(otherTransformation)` is the conjugation `a b a^-1`, which re-expresses the other transformation; it is defined where the result is a transformation of the same kind: a linear transformation or a translation conjugated by a linear transformation, and a translation conjugated by a translation or a proper rigid transformation.

**Points and vectors.**
The rules follow affine geometry (see [Points and vectors](#points-and-vectors)), where `o` is the origin of the coordinate system:

| Operation | Result |
| --- | --- |
| point + vector, vector + point | point |
| point - vector | point |
| point - point | vector |
| point + point, vector - point | error |
| affine combination, e.g. the midpoint `p + (q - p) * 0.5` | point |
| `makeRelative(point)` | the vector `p - o`, with the components of the point |
| `makeAbsolute(vector)` | the point `o + v`, with the components of the vector |
| `makeRelative(vector)`, `makeAbsolute(point)` | error |
| negation, product with a scalar or a quantity, quotient by them | error for points |
| dot, inner, outer and cross product, `cwiseProduct`, `cwiseQuotient` | error for points |
| norm, `inverse()`, `cwiseInverse()`, transpose | error for points |
| absolute tensors other than vectors, e.g. an absolute covector or scalar | error |

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

### The backend interface

The backends adapt other libraries by specializing the traits of the core (`wuzp/core/traits.hpp`), e.g. `Sum` or `Transpose`, for `AccessImplementation<ImplementationT_>`.
These specializations used to be unconstrained, e.g. `AccessValues<AccessImplementation<T>>` for every `T`.
Including both value matrix backends in one translation unit was a redefinition error, and using them in different translation units of one program an undiagnosed violation of the [one definition rule](https://en.cppreference.com/w/cpp/language/definition#One_Definition_Rule).
The core took the backend of a scalar, e.g. of `quantity * 2.0`, from this global default, and nothing told a backend author which traits to specialize: a missing one failed deep inside the core.

Decision:

* A value matrix backend constrains its specializations to its own implementation types, i.e. the matrices and expressions of its library: `EigenArray` (everything derived from `Eigen::ArrayBase`) and `BlitzArray` (`blitz::Array<ValueT, 2>` and its expressions).
  Several value matrix backends can be used in one program, also in one translation unit, side by side; values of different backends cannot be combined.
* Operations derive the values of scalars from the backend of their operands, with the trait `Rebind`, so the core needs no default value matrix backend.
  The aliases that need one, `QuantityI`, `QuantityF` and `QuantityD`, are defined by the backends, e.g. `wuzp_eigen::QuantityD<Length>`.
* A program uses exactly one units backend.
  `AccessUnits` keeps a single specialization for all value types, and the units backends specialize the arithmetic traits for their units, partly for all units (Boost.Units).
  Including two units backends in one translation unit is a redefinition error, and using them in different translation units of one program an ODR violation.
  Quantities of different units backends could not be combined anyway, so constraining the units backends like the value matrix backends would add complexity without a use.
* The interface is defined by concepts in `wuzp/core/concepts.hpp`: `ValueMatrixBackend` checks the traits the core needs, `InvertibleValueMatrixBackend` additionally the matrix inverses and determinants, and `UnitSystem` that the units of a units backend are consistent, e.g. that a velocity is a length divided by a time, and that torque is the unit of energy.
  The value matrix concepts check the traits on scalars, vectors, covectors and square matrices of `double` and `int`.
* The tests check that every backend satisfies its concept (`tests/src/ValueMatrixBackends.cpp`, `tests/src/Unit.cpp`), so a backend that misses a trait fails there, and authors of other backends check theirs the same way, e.g. `static_assert(wuzp::ValueMatrixBackend<my_backend::ValueMatrix>);`.
  The backends do not check it in their headers: the check instantiates many expressions of the backend, which made every translation unit with Eigen compile about 0.7 s longer (see the [benchmarks](benchmarks/README.md#compile-time)), a cost that only backend authors benefit from.
  The core constrains its parameters that take a value matrix backend, e.g. of the tensor aliases and transformations, with the cheap `AnyValueMatrix`, which only checks that the parameter is the `ValueMatrix` of a backend, e.g. not `Eigen::Array`.
  A trait that is not specialized at all fails with the `static_assert` of its primary template, which names it, rather than making the concept unsatisfied: these messages also help users of the core, e.g. when they combine values of different backends.
  Blitz++ does not satisfy `InvertibleValueMatrixBackend` yet (see [Backend support for inverses](#todo-backend-support-for-inverses)).
* Elements are accessed through the trait `Coefficient`, so that a library whose element access differs from `matrix(row, col)` can be adapted.
* The Eigen backend evaluates expressions into arrays with Eigen's default [storage order](https://eigen.tuxfamily.org/dox/group__TopicStorageOrders.html), so that results have one type: e.g. the transpose of a `ValueMatrix<double, 3, 2>` is a `ValueMatrix<double, 2, 3>`, rather than the row-major array of Eigen's `eval()`.

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

### Index variance and order

*wuzp* used to record only the number of contravariant and covariant indices of a tensor, while `transpose`, `innerProduct` and `outerProduct` acted on the matrix of its components, independently of its indices.
The index rules were unsound: the inner product of a vector, a 3x1 matrix, and a covector, a 1x3 matrix, was a 3x3 matrix typed as a tensor of rank 0, the inner product of a transposed vector with a (1,1) tensor contracted two upper indices, and the inner product of a (2,0) and a (1,1) tensor was a (2,0) tensor (#71).

Decision: a tensor records the variance of each index, in their order: `Tensor<Indices<Up, Down>, ...>` is a linear map `T^i_j`, `Indices<Down, Down>` a bilinear form such as a mass matrix, `Indices<Up>` a vector and `Indices<>` a scalar.
The index placement follows chapter 1 of Carroll's [Lecture Notes on General Relativity](https://arxiv.org/abs/gr-qc/9712019), on which his book Spacetime and Geometry is based: an upper index is contravariant, a lower index covariant, and the first index of a tensor of rank 2 is the row of its matrix, the second the column, like `M^i_j` in eq. 1.54, with vectors as columns and covectors as rows (eq. 1.39).
Libraries for symbolic tensor algebra, e.g. SymPy and xAct, likewise record the variance of each index.

* The components are stored in a canonical shape, checked with a `static_assert`: a tensor of rank 0 is a 1x1 matrix, a vector a column, a covector a row, and a tensor of rank 2 a square matrix.
  A tensor with an upper index may also store a batch of vectors or points, one per column, e.g. a point cloud, which the transformations transform at once.
  Transformations, sums, differences, comparisons, and products and quotients with scalars accept batches; the operations that are only defined for a single vector reject them with a `static_assert`: `innerProduct`, `outerProduct`, `dotProduct`, `crossProduct`, `norm()`, `squaredNorm()`, `transpose()`, `inverse()`, `determinant()` and `getInvariant()`.
* `transpose()` swaps the indices of a tensor of rank 2, e.g. `(T^T)_j^i = T^i_j`, and turns a vector, a column, into a covector, a row, and vice versa.
  The latter lowers or raises the index with the Euclidean metric, which keeps the components (Carroll, eq. 1.62 and the text after it; see [Metric](#todo-metric)).
* `innerProduct` contracts the last index of the left-hand side with the first index of the right-hand side, which must be an upper and a lower index, since a contraction pairs an upper with a lower index, and the order of the indices matters (Carroll, eqs. 1.59 and 1.60).
  The only index of a vector or a covector is both its first and its last index, so `innerProduct(w, v)` and `innerProduct(v, w)` are both the scalar `w_i v^i`, and `innerProduct(v, M)` of a vector and a mass matrix is the covector `v^i M_ij`.
  It is computed as the product of the matrices of the components, with a vector on the left-hand side transposed into a row and a covector on the right-hand side into a column.
  The result keeps the remaining indices in their order and is stored in its canonical shape: e.g. `innerProduct(M, v)` is the covector `M_ij v^j`, a row, although the product of the matrices is a column, and the kinetic energy is `innerProduct(v, innerProduct(M, v)) / 2`.
* `outerProduct` concatenates the indices, e.g. `v^i w_j`, so that both operands are vectors or covectors: tensors have rank 2 at most.
* `inverse()`, the matrix inverse of a tensor of rank 0 or 2, maps the indices `(X, Y)` to `(not Y, not X)`, e.g. a metric `g_ij` to `g^ij` (Carroll, eq. 1.56; see [Inverses and determinants](#inverses-and-determinants)).

The matrices of the transformations map one coordinate system into another, possibly of another dimension, so they are not tensors in one coordinate system: the transformations multiply their matrix with the quantities of the tensors they transform, and `getTensor()` only exists for the square matrices of active linear transformations, which are linear maps in their coordinate system; the matrix of a passive linear transformation maps `From` into `To`, a two-point object, so it is only available as quantities.

### Products of tensors

`tensor * tensor` used to be the elementwise (Hadamard) product, also for tensors of rank 2: for a rotation `R` by 90°, `R * R` was `[0 1; 1 0]`, not `-I`.
The result claimed to be a tensor in the same coordinate system, but it did not transform like one, and users of Eigen expect `*` to be the matrix product.
The tensor product (`tensorProduct`) was implemented in neither backend, and its trait gave the result the index counts of the left operand instead of adding them.

Decision: `*` and `/` only take scalars, so that neither the elementwise product nor the matrix product is a silent misunderstanding.

* A tensor can be multiplied by an arithmetic scalar, a quantity (1x1 quantities or values) or a tensor of rank 0, e.g. a scalar field, and divided by them; the result has the indices and the coordinate system of the tensor.
  A tensor of rank 0 must be expressed in the same coordinate system, and its product with a point is rejected, like other products of tensors were.
  Quantities and values of other sizes have no indices, so they are rejected like tensors of rank 1 or more.
* An arithmetic scalar of another type is converted into the value type, if their common type is the value type: `2 * v` is valid for a vector of `double`, `2.5 * v` is rejected for a vector of `int`.
* The products of tensors of rank 1 or more have names: `innerProduct` contracts an index, `outerProduct` is the tensor product, and `cwiseProduct` and `cwiseQuotient` compute the elementwise product and quotient of the components, which depend on the basis, so they return quantities, without indices or a coordinate system.
  This follows Eigen, which distinguishes matrices, whose product is the matrix product, from arrays, whose product is elementwise ([Array class](https://eigen.tuxfamily.org/dox/group__TutorialArrayClass.html)), and mp-units, which defines `*` of vector quantities only with a scalar and names the other products ([character of a quantity](https://mpusz.github.io/mp-units/latest/users_guide/framework_basics/character_of_a_quantity/)).
* `tensorProduct` is removed: `outerProduct` is the tensor product of tensors of rank 2 at most.
* The units of a product of matrices must be equal in all summands of an element, which holds for the matrices of linear maps, whose units are `u_i / v_j` (Hart, Multidimensional Analysis, 1995).

### Points and vectors

The rules for `+` and `-` of points and vectors were correct, but other operations on points compiled although their result depends on the origin of the coordinate system: `point * 2.0`, `point * quantity`, `point / quantity`, `-point`, `point.dotProduct(point)` and `point.inverse()`.
`makeRelative()` of a vector and `makeAbsolute()` of a point were accepted, and tensors of every rank could be absolute, e.g. the aliases `Copoint` and `HomogeneousCopoint`, which have no affine meaning.

Decision: points follow the rules of affine spaces, like CGAL's points and vectors ([2D and 3D Linear Geometry Kernel](https://doc.cgal.org/latest/Kernel_23/index.html)), mp-units' quantity points ([The Affine Space](https://mpusz.github.io/mp-units/latest/users_guide/framework_basics/the_affine_space/)) and Boost.Units' absolute quantities.

* `p - q` is a vector, `p + v`, `v + p` and `p - v` are points.
  Every other operation whose result depends on the origin is rejected with a `static_assert` (see the table in [Conventions](#conventions)): the result must be invariant under translations of the origin, in the terms of Atkey, Johann and Kennedy, ["Abstraction and invariance for algebraically indexed types"](https://doi.org/10.1145/2429069.2429082) (POPL 2013).
* The rules are traits of `Absolute` and `Relative` (`Absolute.hpp`), e.g. `Product<Absolute, Relative>` and `Negation<Absolute>`, which the traits of tensors combine like those of their indices, e.g. `Product<Tensor, Quantities>` with `Product<AbsoluteT, Relative>`, rather than checks in each operator.
* Affine combinations are written `p + (q - p) * t`, e.g. the midpoint `p + (q - p) * 0.5`; there are no barycentric operators.
* `makeRelative()` and `makeAbsolute()` are relative to the origin `o` of the coordinate system: `makeRelative(p)` is the vector `p - o`, `makeAbsolute(v)` the point `o + v`, both with the same components.
  `makeRelative()` rejects a vector, and `makeAbsolute()` a point.
* Only a tensor with one upper index, a vector or a batch of vectors, can be absolute: a covector pairs with vectors, not with points, and the aliases `Copoint` and `HomogeneousCopoint` are removed.
* An absolute scalar, e.g. a time point or a temperature on the Celsius scale, is a point in a one-dimensional affine space, whose coordinate system is its origin, e.g. an epoch, absolute zero or the ice point: `TimePoint` is a point in a one-dimensional coordinate system such as `CoordinateSystem<Epoch>`, the difference of two time points is a `Duration`, and a change of the origin is a one-dimensional translation.
  The SI defines the Celsius temperature like this, `t = T - T0` with `T0 = 273.15 K` ([SI Brochure](https://www.bipm.org/en/publications/si-brochure), section 2.3.1): the thermodynamic temperature `T` has a natural zero, and the Celsius scale moves the origin to the ice point.
  Libraries with absolute scalars give them an origin, too: the clocks of `std::chrono`, the point origins of mp-units and Boost.Units' `absolute`.
  An absolute tensor of rank 0 is rejected, and there are no temperature aliases, since nothing needs them yet.

### Inverses and determinants

`inverse()` computed the elementwise reciprocal, but swapped and flipped the indices like the matrix inverse, which has no tensor meaning: the inverse of a vector was a covector of reciprocals.
`tensorInverse()` computed the matrix inverse.
The check of the units of the inverse, "Matrix is not invertible.", could never fail: for units that do not describe a linear map, computing the candidate inverse already failed with "Units do not match.", and all other units passed the check, which was also wrong in kind, since units cannot tell whether a matrix is invertible.

Decision:

* `inverse()` is the matrix inverse of values, quantities and tensors, like Eigen's `MatrixBase::inverse()`, and `tensorInverse()` is removed.
  The elementwise reciprocal is `cwiseInverse()`, like Eigen's `cwiseInverse()`; for tensors it depends on the basis, so it returns quantities, like `cwiseProduct`.
* The inverse of a tensor exists for rank 0 and 2, and maps the indices `(X, Y)` to `(not Y, not X)`, since `(M^-1)^ij M_jk = δ^i_k`: the inverse of a linear map `T^i_j` is a linear map, and the inverse of a metric or a mass matrix `M_ij` has two upper indices (Carroll, eq. 1.56).
  Vectors, covectors and points have no inverse.
* The units of the inverse are `(M^-1)_ij = b_i / a_j` for `M_ij = a_i / b_j` (Hart, [Multidimensional Analysis](https://doi.org/10.1007/978-1-4612-4208-6), 1995, sections 3.1 and 3.3): the inverse maps the units `a` back to `b`.
  The trait `UnitsDescribeLinearMap` checks that the units of a matrix have this form, `M_ij M_00 = M_i0 M_0j`, i.e. that the unit matrix is the outer product of a column of units and a row of inverse units.
  `inverse()` and `determinant()` check it first, with the message "The units of the matrix do not describe a linear map (M_ij = a_i / b_j).".
  Whether the values are invertible is only known at run time.
* `determinant()` of a tensor is only defined for rank 0 and the mixed tensors of rank 2, `Indices<Up, Down>` and `Indices<Down, Up>`, whose determinant is invariant: `det(A T A^-1) = det(T)`.
  The determinant of a tensor with two lower indices changes by `det(A)^-2` under a linear transformation `A` (`M' = A^-T M A^-1`), and that of a tensor with two upper indices by `det(A)^2`: they are scalar densities of weight -2 and 2, not scalars (Carroll, eq. 2.41).
  It is rejected with a `static_assert`, and `getQuantities().determinant()` computes the determinant of the components; the determinants of values and quantities are not restricted.
  Vectors and covectors have no determinant, like they have no inverse: `determinant()` and `inverse()` exist for every tensor and reject them with a `static_assert`, rather than a constraint, so that the error says why.

### Integer values

Decision: operations whose result is not an integer, such as the norm or a power with a non-integer exponent, are computed in floating point and truncated to the value type, like the norm of integer vectors in Eigen: `pow<1, 2>()` of 5 m² is 2 m (#63).

### Composition and inverse of transformations

Transformations could not be composed or inverted, the most common operations in robotics.
`transformation.transform(otherTransformation)` was a conjugation, which users read as a composition, and two of its cases were wrong:

* A translation returned a linear transformation `Q` unchanged, and a proper rigid transformation `(R, t)` returned `R Q R^-1`, ignoring its translation (#72), but the conjugation `T Q T^-1` maps `x` to `Q x + (I - Q) t`, which is affine.
  For `t = (0, 0, 1)` and the rotation `Q` by `pi` around `x`, it maps `(0, 0, 1)` onto itself, while `Q` maps it to `(0, 0, -1)`.
* A passive transformation re-expressed another passive transformation in its `To`, but the result could only be applied if its coordinate system parameter equaled its `From` (#73).
  That parameter, the coordinate system in which the matrix or vector of a passive transformation was expressed, was redundant: the tensors a passive transformation transforms are expressed in `From`.

Decision:

* `lhs * rhs` composes two transformations: it applies `rhs` first, then `lhs`, like the product of their matrices.
  Passive transformations compose in the order of the [monogram notation of Drake](https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html), `X_AB * X_BC = X_AC`, i.e. `^A_C T = ^A_B T ^B_C T` in the notation of Craig.
  The coordinate systems that meet, `From` of `lhs` and `To` of `rhs`, must be the same, which is checked at compile time with the message "Adjacent frames do not match.", like [wave_geometry](https://github.com/wavelab/wave_geometry) does.
  Active transformations compose within their coordinate system.
* The compositions that are again transformations of *wuzp* are defined, by specializations of the trait `CompositionTraits`: linear transformations with linear transformations (the product of their matrices, e.g. the chain rule for Jacobians, and the composition of Lorentz transformations), translations with translations (the sum of their vectors), and proper rigid transformations with proper rigid transformations and with translations, `(R1, t1) * (R2, t2) = (R1 R2, R1 t2 + t1)` (Sola, Deray and Atchuthan, ["A micro Lie theory for state estimation in robotics"](https://arxiv.org/abs/1812.01537), eq. 171, and Drake's [RigidTransform](https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rigid_transform.html)).
  The result has the type of the aliases, e.g. the product of two `ActiveSpatialRotation3` is an `ActiveSpatialRotation3`, since homogeneous units are given as a single unit.
* Other combinations are rejected with "These transformations cannot be composed.", e.g. a translation with a rotation, and an active with a passive transformation.
  A rotation is an alias of a linear transformation with dimensionless units, so it cannot be told apart from e.g. a scaling or a Lorentz transformation: composing it with a translation into a proper rigid transformation would make the inverse of that transformation, which relies on `R^-1 = R^T`, silently wrong until rotations are a class of their own (see [Validity of rotations and Lorentz transformations](#todo-validity-of-rotations-and-lorentz-transformations)).
* `inverse()` inverts every transformation; a passive inverse transforms from `To` to `From`.
  The inverse of a linear transformation has the matrix inverse, which needs a value matrix backend with matrix inverses (see [Backend support for inverses](#todo-backend-support-for-inverses)); that of a translation is `-t`, and that of a proper rigid transformation `(R^T, -R^T t)` (Sola et al., eq. 170), which only needs the transpose of the rotation and works with every backend.
  The inverses of rotations, `R^T`, and of Lorentz transformations, `eta Lambda^T eta`, which follows from `eta = Lambda^T eta Lambda` (Carroll, eq. 1.14), are the matrix inverses, which `inverse()` computes as long as they are aliases of linear transformations.
* The conjugations of linear transformations by translations and proper rigid transformations, and all transformations of passive transformations by passive transformations, are removed: users compose with `*` and `inverse()`, e.g. `t * q * t.inverse()`, where `q` is a proper rigid transformation that only rotates.
  Passing such a transformation to `transform()` fails with a message that says to compose transformations with `operator*`: `transform()` of a passive transformation checks this before the coordinate system, which passive transformations do not have, and the primary template of `TransformationTraits` for active transformations.
  The other conjugations of active transformations remain, since their results are transformations of the same kind: `A B A^-1` of a linear transformation `B` by a linear transformation `A`, and the conjugations of a translation by `t`, which are the translation by `A t` for a linear transformation `A`, by `R t` for a proper rigid transformation `(R, t')`, and by `t` for a translation.
* The passive transformations have no coordinate system parameter: they transform tensors expressed in `From`, and the vector of a passive translation is expressed in `To`.
  Passive linear transformations have no `getTensor()`: their matrix maps `From` into `To`, a two-point object, which typing it as a tensor in `From` would misrepresent; `getQuantities()` returns it.
* `isApprox(other, precision)` of transformations is `const` and takes a precision with the default of tensors (#68).
  It compares transformations of the same kind, expressed in the same coordinate system (active) or between the same coordinate systems (passive), and compares the matrices of linear transformations, the vectors of translations, and both the rotation and the translation of proper rigid transformations.
* The base class `Transformation` has a protected non-virtual destructor rather than a virtual one: transformations are values that are never deleted through a pointer to the base class, and the pointer to the virtual table made every transformation larger.

## Review of mathematical and physical soundness (November 2026)

The library was reviewed for mathematical and physical soundness, with claims verified by compiling and running programs.

**Sound:**

* The unit exponent algebra and the canonical reduction of rational exponents.
* The units of the matrix inverse, `(M^-1)_ij = b_i / a_j` for the matrices with units of the form `M_ij = a_i / b_j`, i.e. matrices that describe a linear map; the units of other matrices are rejected (see [Inverses and determinants](#inverses-and-determinants)).
* The transformation rules for vectors, points, (1,1) and (0,2) tensors (`A^-T M A^-1` matches `R M R^T` to 1e-15 for a general rotation).
* Active and passive transformations with the same parameters cancel each other (for proper rigid transformations only with the translation `-R^T t` of the passive one, see [Conventions](#conventions)); the order of rotation and translation in rigid transformations; translations moving points but not vectors.
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
| The index rules of `innerProduct` were unsound, e.g. the inner product of a vector and a covector was a 3x3 matrix typed as a scalar | #71 |
| `isApprox` of transformations was not `const`, had no precision, did not check `From` and `To`, and did not work for rigid transformations | #68 |
| Translations and rigid transformations conjugated linear transformations into linear transformations, ignoring the translation, which is wrong for points | #72 |
| Passive transformations transformed by passive transformations could not be applied | #73 |

## Open design questions

### TODO: Jacobians

A Jacobian is a `PassiveLinearTransformation`, so it also maps points: the Jacobian of polar to Cartesian coordinates at `r = 2` maps the point `(2 m, π/2)` to `(2, 3.14) m` instead of `(0, 2) m`.
A Jacobian is a local linearization and must only map tangent vectors.
The rule `A^-T M A^-1` cannot handle non-square Jacobians, and the check of the mass matrix test (`TestJacobian.TransformMassMatrix`) is commented out.
The physically meaningful operations are the pullback `M_q = J^T M_x J` (no inverse needed) and the operational space inertia `Λ = (J M^-1 J^T)^-1`.
The composition of Jacobians is the chain rule, `J_AB * J_BC = J_AC` (see [Composition and inverse of transformations](#composition-and-inverse-of-transformations)).

Recommendation: a separate Jacobian type that rejects points and provides the pullback.

### TODO: Metric

`dotProduct` and `norm` use the Euclidean metric and contract two vectors, i.e. two upper indices, and `transpose()` of a vector or a covector lowers or raises its index with it.
This is fine in orthonormal coordinate systems, but not invariant under Jacobians or Lorentz transformations: the Euclidean "dot product" of a 4-vector changes from 1010100 to 1249730 m² under a boost, while the Minkowski invariant is preserved.
`SpacetimeVector` is just a 4D `PositionVector`.

Options: (a) a metric as part of the coordinate system, used by `dotProduct` and `norm`; (b) restrict `dotProduct` and `norm` to Euclidean coordinate systems and provide explicit metric contractions; (c) document the limitation.
Recommendation: (c) for 1.0, (a) later.

### TODO: Validity of rotations and Lorentz transformations

Rotations are not checked for orthogonality or a determinant of 1, so a shear is accepted as a rotation.
The transformation rules use the general inverse and remain correct for any invertible matrix; only the guarantees of a rotation (e.g. preserving norms) break.
The inverse of a proper rigid transformation, `(R^T, -R^T t)`, relies on `R^-1 = R^T`, though, and is wrong for a rotation that is not one.
Rotations are aliases of linear transformations with dimensionless units, so they cannot be told apart from e.g. scalings or Lorentz transformations: their `inverse()` is the matrix inverse rather than `R^T`, which needs a backend with matrix inverses, and they do not compose with translations into proper rigid transformations.
Likewise, Lorentz transformations are aliases of passive linear transformations: their `inverse()` is the matrix inverse rather than `eta Lambda^T eta`, and nothing checks `Lambda^T eta Lambda = eta`.

Decision: a rotation class and a Lorentz class, with a runtime check in debug builds when constructing them (`R^T R = I` and `det R = 1`, `Lambda^T eta Lambda = eta`).
Their inverses are structural, `R^T` and `eta Lambda^T eta`, so they work with every value matrix backend.
Rotations will compose with translations and proper rigid transformations into proper rigid transformations (rotation * translation, translation * rotation, rigid * rotation and rotation * rigid), which are rejected until then.

### TODO: Moving coordinate systems

Coordinate systems are axes and an origin, not moving observers.
A passive transformation of a velocity re-expresses it; it does not add `ω × r` or the velocity of the coordinate system, and a rigid transformation of a torque ignores the moment arm (`t × F`).
The rigid transformation of a mass or inertia tensor does not compile, which is arguably safer than guessing the parallel axis term.
The transport formulas exist only in the tests.
Recommendation: document this clearly; consider dedicated twist and wrench types later.

### TODO: Backend support for inverses

Blitz++ provides neither matrix inverses nor determinants, so transformations of covectors, linear maps and bilinear forms, and the inverses of linear transformations, only work with Eigen, and the Blitz++ backend does not satisfy `InvertibleValueMatrixBackend`.
The inverses of translations and proper rigid transformations need no matrix inverse and work with every backend, and so will those of the planned rotation and Lorentz classes (see [Validity of rotations and Lorentz transformations](#todo-validity-of-rotations-and-lorentz-transformations)).
Of the inverses, the Blitz++ backend only implements the elementwise reciprocal, `cwiseInverse()`.
This weakens the goal that every feature works with every backend.
Recommendation: implement small fixed-size inverses for Blitz++ in *wuzp*, or document the restriction per backend.

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
