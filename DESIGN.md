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
  A coordinate system means axes, an origin and a metric, Euclidean by default or Minkowski for spacetime (see [Metric](#metric)); it is not a moving observer (see [Moving frames](#moving-frames)).
* **Points and vectors.** Absolute tensors are points, relative tensors are vectors and all other tensors (like temperatures and temperature differences).
  Points follow the rules of affine spaces: operations whose result depends on the origin of the coordinate system are rejected (see [Points and vectors](#points-and-vectors)).
* **Transformations.** Rotations, translations and proper rigid transformations (first rotation, then translation), each in an active and a passive form, Lorentz transformations, and Jacobians, which push forward vectors and pull back covectors (see [Jacobians](#jacobians)).
  They compose with `*`, in the order of Drake's monogram notation, and have an `inverse()` (see [Composition and inverse of transformations](#composition-and-inverse-of-transformations)).
  Rotations and Lorentz transformations are classes of their own, which check their validity in debug builds and invert without a matrix inverse (see [Validity of rotations and Lorentz transformations](#validity-of-rotations-and-lorentz-transformations)).

## Conventions

**Transformation rules.**
For a linear transformation with component matrix `A` (contravariant components transform as `v' = A v`):

| Object | Indices | Rule | Implemented |
| --- | --- | --- | --- |
| Vector | `Indices<Up>` | `v' = A v` | yes |
| Point | `Indices<Up>`, absolute | `p' = A p` (via a relative intermediate) | yes |
| Covector | `Indices<Down>` | `w' = w A^-1` | yes |
| Linear map | `Indices<Up, Down>` | `T' = A T A^-1` | yes |
| Bilinear form, e.g. mass matrix | `Indices<Down, Down>` | `M' = A^-T M A^-1` | yes |
| E.g. covariance matrix | `Indices<Up, Up>` | `T' = A T A^T` | yes |

`A^-1` appears twice in the rule for bilinear forms because `A` transforms components, so covariant indices transform with `A^-1`.
Equivalently, `A^-1` is the change-of-basis matrix `P` used by Wikipedia ("old = P new").
The notes at the end of this document start with the opposite convention (`M' = A^-1 M A`, `A^T M A`); the code follows the table above.

Translations move points, leave vectors, covectors and tensors with two upper indices unchanged, and reject the other tensors, which may depend on the origin (see [Translations of tensors](#translations-of-tensors)).
A proper rigid transformation first rotates, then translates: `x -> R x + t`, so it only rotates the tensors that translations leave unchanged.
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

A Jacobian `J` of a change of coordinates from `From` to `To` maps tangent objects, not points: `transform()` pushes vectors and tensors with two upper indices forward, `pullback()` pulls covectors and tensors with two lower indices back (see [Jacobians](#jacobians)):

| Object | Indices | `transform()`, from `From` to `To` | `pullback()`, from `To` to `From` |
| --- | --- | --- | --- |
| Vector, e.g. a velocity | `Indices<Up>` | `v' = J v` | error |
| Point | `Indices<Up>`, absolute | error | error |
| Covector, e.g. a force | `Indices<Down>` | error | `w' = w J`, i.e. `J^T w` for columns |
| E.g. a covariance matrix | `Indices<Up, Up>` | `T' = J T J^T` | error |
| E.g. a mass matrix | `Indices<Down, Down>` | error | `M' = J^T M J` |
| Linear map, scalar | `Indices<Up, Down>`, `Indices<Down, Up>`, `Indices<>` | error | error |

Transformations compose with `*` and have an `inverse()`, except Jacobians that are not square (see [Composition and inverse of transformations](#composition-and-inverse-of-transformations)).
`activeTransformation.transform(otherTransformation)` is the conjugation `a b a^-1`, which re-expresses the other transformation; it is defined where the result is a transformation of the same kind: a linear transformation or a translation conjugated by a linear transformation or a rotation, a rotation conjugated by a rotation, and a translation conjugated by a translation or a proper rigid transformation.

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

"For reasons of history and convention, plane and solid angles are treated within the SI as quantities with the unit one" ([SI Brochure](https://www.bipm.org/en/publications/si-brochure), 9th edition, section 2.3.3, in the wording of version 3.01 of 2024 and later; the original version of 2019 says that they are "in effect also treated within the SI as quantities with the unit one"): the radian is the coherent unit of plane angle, and `rad = 1`, although the current version (4.01 of 2026) notes that an angle "is not the same kind of quantity as other length ratios" (Table 4, note b).
Boost.Units treats the plane angle as a dimension of its own, and mp-units keeps the radian as a unit apart from one.
Following them would force explicit radians into ordinary formulas: `v + ω × r` only compiles after dividing `ω × r` by one radian, torque becomes `N m / rad`, and code written for one units backend does not compile with another.

Decision: angles are quantities with the unit one, i.e. dimensionless, in every units backend, like the SI treats them.
The Boost.Units and mp-units adapters define `Angle` as dimensionless, angular velocity as `1/s`, angular acceleration as `1/s^2` and torque as `N m`.

The price, the known risks of treating angles as quantities with the unit one:

* `rad/s` cannot be told apart from `Hz`, so *wuzp* does not catch a missing factor of 2π between an angular velocity and a frequency, against which the SI Brochure warns: "It is especially important to carefully distinguish frequencies from angular frequencies, because by definition their numerical values differ by a factor of 2π" (section 2.3.4).
* Torque cannot be told apart from energy, both `N m`, although "the joule is never used for expressing torque" (section 2.3.4).
* `sin()` and `cos()` take any dimensionless quantity, e.g. also a ratio of two lengths, not only an angle.

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
* Operations derive the values of scalars from the backend of their operands, with the trait `Rebind`, which replaces `AccessValues`, so the core needs no default value matrix backend.
  The aliases that need one, `QuantityI`, `QuantityF` and `QuantityD`, are defined by the backends, e.g. `wuzp_eigen::QuantityD<Length>`.
* A program uses exactly one units backend.
  `AccessUnits` keeps a single specialization for all arithmetic value types, and the units backends specialize the arithmetic traits for their units, partly for all units (Boost.Units).
  The core defines the sum and the difference of units, for every units backend: they only exist for equal units, which every backend gives the same type, and are rejected with "Units do not match." otherwise.
  Including two units backends in one translation unit is a redefinition error, and using them in different translation units of one program an ODR violation.
  Quantities of different units backends could not be combined anyway, so constraining the units backends like the value matrix backends would add complexity without a use.
* The interface is defined by concepts in `wuzp/core/concepts.hpp`: `ValueMatrixBackend` checks the traits the core needs, `InvertibleValueMatrixBackend` additionally the matrix inverses and determinants, and `UnitSystem` that the units of a units backend are consistent, e.g. that a velocity is a length divided by a time, that torque is the unit of energy, and that the square root of an area is a length.
  The value matrix concepts check the traits on scalars, vectors, covectors and square matrices of `double` and `int`.
* The tests check that every backend satisfies its concept (`tests/src/ValueMatrixBackends.cpp`, `tests/src/Unit.cpp`), so a backend that misses a trait fails there, and authors of other backends check theirs the same way, e.g. `static_assert(wuzp::ValueMatrixBackend<my_backend::ValueMatrix>);`.
  The backends do not check it in their headers: the check instantiates many expressions of the backend, which made every translation unit with Eigen compile about 0.7 s longer (see the [benchmarks](benchmarks/README.md#compile-time)), a cost that only backend authors benefit from.
  The core constrains its parameters that take a value matrix backend, e.g. of the tensor aliases and transformations, with the cheap `AnyValueMatrix`, which only checks that the parameter is the `ValueMatrix` of a backend, e.g. not `Eigen::Array`.
  A trait that is not specialized at all fails with the `static_assert` of its primary template, which names it, rather than making the concept unsatisfied: these messages also help users of the core, e.g. when they combine values of different backends.
  Both backends satisfy `InvertibleValueMatrixBackend`; the Blitz++ backend computes the inverses and determinants itself (see [Backend support for inverses](#backend-support-for-inverses)).
* Elements are accessed through the trait `Coefficient`, so that a library whose element access differs from `matrix(row, col)` can be adapted.
* The tests run against *archetypes*, minimal backends that provide exactly what the concepts require, like the archetypes of [Boost's Concept Check library](https://www.boost.org/doc/libs/release/libs/concept_check/doc/reference.htm#basic-archetype): a value matrix backend of fixed-size arrays that cannot be copied or assigned, with eager evaluation and an element access of its own, and a units backend with integer exponents of time, length and mass, whose `AccessUnits` only takes arithmetic value types (`tests/src/archetype_values`, `tests/src/archetype_units`, the presets `*-archetype-archetype`).
  If the core used anything that the concepts do not promise, the tests would fail with them.
  They found that the core used more, which is fixed: the constructor of values passed the elements to `Initialize` with their own types, e.g. `const int&`, while the concept checks values of the value type, so the core converts them now; `sin()` and `cos()` looked up the units of a value matrix instead of its value type; `pow<P, Q>()` needs rational powers of units and `determinant()` the determinant of integer matrices, which the concepts did not check; and the core relied on each units backend to reject the sum of different units, which the concepts cannot check, so the core rejects it itself.
  The concepts also require now that the matrices can be default constructed and moved, and constructed from the lazy results of the traits, which must be move constructible, as the core does, and check `Random` with a disjunction that Clang 14 evaluates correctly.
  See "Writing a backend" in [CONTRIBUTING.md](CONTRIBUTING.md#writing-a-backend).
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
  The latter lowers or raises the index with the Euclidean metric, which keeps the components (Carroll, eq. 1.62 and the text after it), so it requires a Euclidean coordinate system (see [Metric](#metric)).
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
  The elements that values, quantities and tensors are constructed from, or that `set()` sets, follow the same rule: `Vector{1, 2, 3}` is valid for a vector of `double`, `Vector{1.5, 2.0, 3.0}` is rejected for a vector of `int`, which truncated it before, and for a vector of `float`, whose elements are written `1.5F`.
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
* The inverse of an integer matrix is rejected with "The inverse of an integer matrix is not an integer matrix. Use a floating-point value type.", in `ValuesWrapper::inverse()`, so with every value matrix backend: it is not an integer matrix in general, and a backend would truncate it, e.g. Eigen divides integers.
  The determinant of an integer matrix is an integer, and is allowed.
* `determinant()` of a tensor is only defined for rank 0 and the mixed tensors of rank 2, `Indices<Up, Down>` and `Indices<Down, Up>`, whose determinant is invariant: `det(A T A^-1) = det(T)`.
  The determinant of a tensor with two lower indices changes by `det(A)^-2` under a linear transformation `A` (`M' = A^-T M A^-1`), and that of a tensor with two upper indices by `det(A)^2`: they are scalar densities of weight -2 and 2, not scalars (Carroll, eq. 2.41).
  It is rejected with a `static_assert`, and `getQuantities().determinant()` computes the determinant of the components; the determinants of values and quantities are not restricted.
  Vectors and covectors have no determinant, like they have no inverse: `determinant()` and `inverse()` exist for every tensor and reject them with a `static_assert`, rather than a constraint, so that the error says why.

### Integer values

Decision: operations whose result is not an integer, such as the norm or a power with a non-integer exponent, are computed in floating point and truncated to the value type, like the norm of integer vectors in Eigen: `pow<1, 2>()` of 5 m² is 2 m (#63).
The matrix inverse is rejected instead, since a truncated inverse does not invert the matrix (see [Inverses and determinants](#inverses-and-determinants)).

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
* The compositions that are again transformations of *wuzp* are defined, by specializations of the trait `CompositionTraits`: linear transformations with linear transformations (the product of their matrices), Jacobians with Jacobians and with passive linear transformations, including rotations and Lorentz transformations, into Jacobians (likewise, the chain rule, see [Jacobians](#jacobians)), rotations with rotations and Lorentz transformations with Lorentz transformations (likewise, but of their own type), translations with translations (the sum of their vectors), and proper rigid transformations, rotations and translations with each other into proper rigid transformations, `(R1, t1) * (R2, t2) = (R1 R2, R1 t2 + t1)`, where a rotation `R` is `(R, 0)` and a translation `t` is `(I, t)` (Sola, Deray and Atchuthan, ["A micro Lie theory for state estimation in robotics"](https://arxiv.org/abs/1812.01537), eq. 171, and Drake's [RigidTransform](https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rigid_transform.html)).
  The result of linear transformations has the type of the aliases, e.g. the product of two dimensionless linear transformations has a single dimensionless unit, since homogeneous units are given as a single unit.
  A rotation or a Lorentz transformation is a linear transformation, so its compositions with linear transformations are linear transformations.
  The compositions of rotations with translations and proper rigid transformations are defined in the headers of the rotations, which include the class templates of the proper rigid transformations (`ActiveProperRigidTransformationTemplate.hpp`, `PassiveProperRigidTransformationTemplate.hpp`).
* Other combinations are rejected with "These transformations cannot be composed.", e.g. a translation with a linear transformation that may not be a rotation, e.g. a scaling, whose composition is affine but not rigid, and an active with a passive transformation.
* `inverse()` inverts every transformation, except Jacobians that are not square (see [Jacobians](#jacobians)); a passive inverse transforms from `To` to `From`.
  The inverse of a linear transformation has the matrix inverse, which needs a value matrix backend with matrix inverses, which both backends are (see [Backend support for inverses](#backend-support-for-inverses)); that of a translation is `-t`, and that of a proper rigid transformation `(R^T, -R^T t)` (Sola et al., eq. 170), which only needs the transpose of the rotation.
  The inverses of rotations, `R^T`, and of Lorentz transformations, `eta Lambda^T eta`, which follows from `eta = Lambda^T eta Lambda` (Carroll, eq. 1.14), are structural: they need no matrix inverse, and add no rounding errors.
* The conjugations of linear transformations by translations and proper rigid transformations, and all transformations of passive transformations by passive transformations, are removed: users compose with `*` and `inverse()`, e.g. `t * r * t.inverse()` for a rotation `r`.
  Passing such a transformation to `transform()` fails with a message that says to compose transformations with `operator*`: `transform()` of a passive transformation checks this before the coordinate system, which passive transformations do not have, and the primary template of `TransformationTraits` for active transformations.
  The other conjugations of active transformations remain, since their results are transformations of the same kind: `A B A^-1` of a linear transformation `B` by a linear transformation or a rotation `A`, `R Q R^T` of a rotation `Q` by a rotation `R`, and the conjugations of a translation by `t`, which are the translation by `A t` for a linear transformation or a rotation `A`, by `R t` for a proper rigid transformation `(R, t')`, and by `t` for a translation.
  The conjugation `A R A^-1` of a rotation by a linear transformation, which is not a rotation in general, is rejected: users compose `a * r * a.inverse()`.
* The passive transformations have no coordinate system parameter: they transform tensors expressed in `From`, and the vector of a passive translation is expressed in `To`.
  Passive linear transformations have no `getTensor()`: their matrix maps `From` into `To`, a two-point object, which typing it as a tensor in `From` would misrepresent; `getQuantities()` returns it.
* `isApprox(other, precision)` of transformations is `const` and takes a precision with the default of tensors (#68).
  It compares transformations of the same kind, expressed in the same coordinate system (active) or between the same coordinate systems (passive), and compares the matrices of linear transformations, the vectors of translations, and both the rotation and the translation of proper rigid transformations.
* The base class `Transformation` has a protected non-virtual destructor rather than a virtual one: transformations are values that are never deleted through a pointer to the base class, and the pointer to the virtual table made every transformation larger.

### Validity of rotations and Lorentz transformations

Rotations and Lorentz transformations were aliases of linear transformations with dimensionless units, so they could not be told apart from e.g. scalings, and nothing checked them: a shear was accepted as a rotation, which stretched a unit vector to a length of 5.1.
Their `inverse()` was the general matrix inverse, rather than `R^T` and `eta Lambda^T eta`, so it needed a backend with matrix inverses, and rotations could not compose with translations into proper rigid transformations, whose inverse `(R^T, -R^T t)` relies on `R^-1 = R^T`.

Libraries differ in how they check rotations:
Drake's [RotationMatrix](https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rotation_matrix.html) checks `max_ij |(R R^T - I)_ij| <= 128 eps` and `det(R) > 0` only in debug builds, throws if the check fails, and provides `MakeUnchecked()` and `ProjectToRotationMatrix()`; [Sophus](https://github.com/strasdat/Sophus) checks that the Frobenius norm of `R R^T - I` is below `1e-10` and `det(R) > 0` in every build and aborts, with a projection by a singular value decomposition; Eigen checks nothing.

Decision:

* `ActiveSpatialRotation` and `PassiveSpatialRotation` are classes, in every dimension, with the same template parameters as the aliases they replace.
  A rotation is valid if `max_ij |(R R^T - I)_ij| <= tolerance` and `det(R) > 0`, like Drake's: the determinant rules out reflections, which are orthogonal too.
  The determinant is `ad - bc` in two dimensions and the triple product `c0 . (c1 x c2)` of the columns in three dimensions, which only need the cross and dot products of every value matrix backend; other dimensions use the determinant of the backend.
* `LorentzTransformation` is a class, valid if `max_ij |(Lambda^T eta Lambda - eta)_ij| <= tolerance` for the Minkowski metric `eta = diag(1, -1, ..., -1)`, the definition of Lorentz transformations (Carroll, [Lecture Notes on General Relativity](https://arxiv.org/abs/gr-qc/9712019), eq. 1.14, whose signature `(-, +, +, +)` has the metric `-eta`, which gives the same condition), which includes the improper and the non-orthochronous ones, e.g. the reflection of time.
  `createBoostMatrix()` remains, and creates boosts without checking them: they are valid by construction, and the rounding errors of very large Lorentz factors could exceed the tolerance.
* The tolerance is the trait `ValidityTolerance<ValueT>`, separate from `Precision`, the default of `isApprox()`, which is too strict for transformations whose rounding errors accumulated, e.g. in compositions: `1e-9` for `double`, `1e-4` for `float`, and `0` for `int`.
  Drake accepts 128 machine epsilons, about `3e-14`, Sophus `1e-10`; *wuzp* leaves a margin for longer chains of compositions.
  `isValid(tolerance)` checks with another tolerance, and other value types specialize the trait.
* The constructors check the validity in debug builds, i.e. if `NDEBUG` is not defined, like `assert()`, and throw `std::invalid_argument` if the check fails, like Drake and like `createBoostMatrix()`, which throws for velocities of at least the speed of light: an exception can be tested, unlike an abort.
  Release builds do not check, so the checks cost nothing in the [benchmarks](benchmarks/README.md), which build in release mode; like with `assert()`, all translation units of a program must agree on `NDEBUG`.
  The NDEBUG switch is in one place, `wuzp::internal::checksValidity` (`wuzp/trafo/internal/validity.hpp`).
* `makeUnchecked()` skips the check, e.g. in hot paths, like Drake's `MakeUnchecked()`.
  The compositions, the inverses and the conjugations of rotations and Lorentz transformations, and the rotations of compositions and inverses of proper rigid transformations, are valid by construction, up to rounding, and use it.
* The default constructors create the identity, like Drake's, so that a rotation is always valid.
* `projectToRotation()` computes the rotation nearest to a matrix, the orthogonal factor of its polar decomposition, by the Newton-Schulz iteration `X <- X (3 I - X^T X) / 2`, which only needs matrix products and therefore works with every backend, unlike a singular value decomposition.
  It converges quadratically if all singular values are in `(0, sqrt(3))` (Higham, Functions of Matrices, SIAM 2008, section 8.3), which `||I - A^T A|| < 1` guarantees (Björck and Bowie, "An iterative algorithm for computing the best estimate of an orthogonal matrix", SIAM J. Numer. Anal. 8(2), 1971): the projection requires this in the Frobenius norm and a positive determinant, and throws `std::invalid_argument` otherwise, in every build.
* The inverses are structural, `R^T` and `eta Lambda^T eta`, so they work with every value matrix backend and add no rounding errors.
* A rotation or a Lorentz transformation transforms the same tensors as the linear transformation with its matrix, whose traits it inherits, and composes with linear transformations into linear transformations.
  The traits take `A^-1` from `inverse()` of the transformation, so a rotation or a Lorentz transformation transforms covectors, linear maps and bilinear forms with its structural inverse: without the rounding errors of a matrix inverse, and also with integer matrices, whose matrix inverse is rejected.
  A rotation composes with rotations into rotations, and with translations and proper rigid transformations into proper rigid transformations, a Lorentz transformation with Lorentz transformations into Lorentz transformations (see [Composition and inverse of transformations](#composition-and-inverse-of-transformations)).
* The rotation of a proper rigid transformation must be a rotation (the concept `AnyRotation`), so that the inverse `(R^T, -R^T t)` is right.
* Orthogonality and the Minkowski condition are defined by a metric, so the rotations and the proper rigid transformations take Euclidean coordinate systems, and the Lorentz transformations Minkowski ones (see [Metric](#metric)).

### Backend support for inverses

Blitz++ provides neither matrix inverses nor determinants, so transformations of covectors, linear maps and bilinear forms, and the inverses of linear transformations, only worked with Eigen, and the Blitz++ backend did not satisfy `InvertibleValueMatrixBackend`.

Decision: the Blitz++ backend computes the matrix inverse and the determinant by Gauss-Jordan elimination with partial pivoting (Golub and Van Loan, Matrix Computations, 4th ed., 2013, sections 3.2 and 3.4; Press et al., Numerical Recipes, 3rd ed., 2007, section 2.1), in its traits `Inverse` and `Determinant`, without LAPACK.
It is linear algebra, so it belongs in the backend, not in the core.

* The determinant is the product of the pivots of the same elimination, with the sign of the row swaps.
  The determinant of an integer matrix is computed in `double` and rounded to the nearest integer, like Eigen's, which computes the determinants of small integer matrices exactly.
* The inverse of an integer matrix is rejected by the core, with every backend (see [Inverses and determinants](#inverses-and-determinants)), so the backend only inverts floating point matrices.
* A matrix is singular if a pivot is exactly zero: then the determinant is zero and the inverse is NaN, like the non-finite elements of Eigen's inverse of a singular matrix.
  A matrix that is singular only up to rounding has a tiny pivot: its inverse has huge elements, with both backends.
* The tests compare the inverses and the determinants of both backends with values computed by hand, and with each other for well and badly conditioned matrices of the sizes 1 to 4, also with zeros on the diagonal, which need row swaps (`tests/src/ValueMatrixBackends.cpp`).

### Jacobians

A Jacobian was an alias of `PassiveLinearTransformation`, so it transformed everything that a change of basis transforms.
It mapped points: the Jacobian of the Cartesian coordinates with respect to the polar coordinates at the point `(2 m, π/2)` mapped that point to `(-3.14, 2) m` instead of `(0, 2) m`.
It transformed covectors and tensors with two lower indices with its inverse, `w J^-1` and `J^-T M J^-1`, which only exists for square Jacobians, so the mass matrix test was commented out, and the physically meaningful operations on them, the pullbacks `w J` and `J^T M J`, did not exist.

A Jacobian `J = d(To)/d(From)` of a change of coordinates, e.g. of the task space coordinates of a robot with respect to its joint coordinates, is its linearization at one point, possibly between coordinate systems of different dimensions.
It maps the tangent objects at that point, not the points: contravariant tensors are pushed forward from `From` to `To`, covariant ones are pulled back from `To` to `From` (Lee, Introduction to Smooth Manifolds, 2nd ed., Springer, 2013, chapters 3, 11 and 12; [Pullback (differential geometry)](https://en.wikipedia.org/wiki/Pullback_(differential_geometry))).

Decision: `Jacobian<ValuesT, ValueT, From, To, DimensionFrom, DimensionTo, Units...>` is a class of its own, with the template parameters of the alias, which maps the tensors as in the table in [Conventions](#conventions).
Its matrix `J_ij = dx_i / dq_j`, for the coordinates `q` of `From` and `x` of `To`, is stored as quantities with the units `[x_i] / [q_j]`.

* `transform()` is the pushforward of relative vectors, `v' = J v`, also of batches, and of tensors with two upper indices, `T' = J T J^T`, e.g. of a covariance `S_x = J S_q J^T`, like the passive linear transformation with the matrix of the Jacobian.
  It rejects points with "A Jacobian maps vectors, not points: the change of coordinates whose derivative it is maps the points.", and every other tensor with "A Jacobian pushes forward vectors and tensors with two upper indices. Use pullback() for covectors and tensors with two lower indices.".
* `pullback()` pulls covectors, `w' = w J`, and tensors with two lower indices, `M' = J^T M J`, back from `To` to `From`, and rejects every other tensor with "The pullback by a Jacobian maps covectors and tensors with two lower indices. Use transform() for vectors and tensors with two upper indices.".
  It needs no inverse, so it also works for Jacobians that are not square, e.g. for the joint torques `τ = J^T F` that balance a force `F` at the end effector, a covector in task space, or for the joint space inertia `J^T M J` of a mass matrix `M` in Cartesian coordinates.
  The test of the rotating dumbbell checks that the pullback of the diagonal mass matrix of the Cartesian coordinates of both masses is the mass matrix of its minimal coordinates (`tests/src/RotatingDumbbell.cpp`).
* The units of the results follow from those of the matrix products: e.g. the pullback of a force in `N` by the Jacobian of the Cartesian coordinates with respect to the polar coordinates is a force in `N` along `r` and a torque in `N m` about the origin, and the pushforward of a covariance of a length and an angle is a covariance in `m^2` (`tests/src/Jacobian.cpp`).
* A mixed tensor, e.g. a linear map, would need its upper index pushed forward and its lower index pulled back, in opposite directions, so both reject it, and scalars too, like the linear transformations do.
  A square Jacobian that is a change of basis can be turned into a `PassiveLinearTransformation`, which transforms every tensor: `PassiveLinearTransformation<...>{jacobian.getQuantities()}`.
* Jacobians compose with Jacobians by the chain rule, `J_AC = J_AB * J_BC`, in the order of Drake's monogram notation, and the coordinate systems that meet must match ("Adjacent frames do not match."), like for the other passive transformations.
  A change of basis, i.e. a passive linear transformation, a rotation or a Lorentz transformation, is the Jacobian of the identity map between two coordinate systems, so its compositions with Jacobians, in both orders, are Jacobians too, by the chain rule: e.g. `R_EW * J_W` expresses the task space Jacobian `J_W` in the coordinate system `E` instead of `W`.
  The trait `internal::IsChangeOfBasis` marks the changes of basis.
  Jacobians do not compose with translations or proper rigid transformations, which move points: "These transformations cannot be composed.".
* A passive linear transformation is a change of basis, so its matrix is square: `PassiveLinearTransformation<ValuesT, ValueT, From, To, Dimension, Units...>` has one dimension, like the rotations and the Lorentz transformations, instead of the two of the alias of the Jacobian.
  A linear map between coordinate systems of different dimensions is a Jacobian.
  Jacobians and linear transformations, active and passive, share the pushforward of vectors and tensors with two upper indices (`internal::pushForward()`), which linear transformations also apply to points, and the pullback of covectors and tensors with two lower indices (`internal::pullBack()`): a linear transformation `A` transforms them by pulling them back by its inverse, `w A^-1` and `A^-T M A^-1`, which for a change of basis maps `To` into `From`, so the formulas exist once.
* `inverse()`, the Jacobian of the inverse change of coordinates, is the matrix inverse, which both value matrix backends provide (see [Backend support for inverses](#backend-support-for-inverses)).
  Only square Jacobians have one: the others are rejected with "Only a square Jacobian, between coordinate systems of the same dimension, has an inverse.".
* The operational space inertia of Khatib, `Λ = (J M^-1 J^T)^-1` for the joint space inertia `M` and a Jacobian `J` of full row rank ("A unified approach for motion and force control of robot manipulators: The operational space formulation", IEEE Journal on Robotics and Automation 3(1), 1987, [doi:10.1109/JRA.1987.1087068](https://doi.org/10.1109/JRA.1987.1087068); Featherstone and Orin, "Dynamics", in Siciliano and Khatib (eds.), Springer Handbook of Robotics, Springer, 2008, section 2.5.4, "Operational-Space Inertia Matrix"), needs no operation of its own: `M.inverse()` has two upper indices (see [Inverses and determinants](#inverses-and-determinants)), `transform()` pushes it forward into the task space, and its inverse has two lower indices again, `jacobian.transform(massMatrix.inverse()).inverse()`.
  The test of the rotating dumbbell checks it at one mass against the closed form `(m1 + m2) u u^T + m1 n n^T`, for the direction `u` of the rod and its normal `n`: the massless rod only transmits forces along itself.
* The matrix maps one coordinate system into another, so it is not a tensor in one coordinate system: `getQuantities()` returns it, and there is no `getTensor()`, like for the passive linear transformations.

### Translations of tensors

Translations only transformed points and vectors.
Covectors and tensors with two upper indices, e.g. covariances, failed with "'TransformationTraits' trait not specialized.", and so did proper rigid transformations, although their rotations transform them.

The Jacobian of a translation `x -> x + t` is the identity, so it leaves the tensors at a point, whose components do not depend on the origin, unchanged, and only moves the points.
Some quantities are about a point rather than at one, though, e.g. the inertia of a body about the origin, which changes with the origin by the parallel axis theorem, `I_o = I_c + m (|c|^2 1 - c c^T)` for the inertia `I_c` about the center of mass `c`, and the type of a tensor cannot tell them apart.

Decision: translations decide by the variance of the indices, conservatively:

* They leave vectors, covectors, e.g. gradients and forces as covectors, and tensors with two upper indices, e.g. covariances, which are about the mean, unchanged, and move points (`internal::isTranslationInvariant`).
  Proper rigid transformations therefore transform vectors, covectors and tensors with two upper indices by their rotation alone, e.g. `w R^T` and `R S R^T`.
* They reject the other tensors with "A translation only transforms points, vectors, covectors and tensors with two upper indices: other tensors may depend on the origin, e.g. an inertia about it.", since for each of them a common quantity depends on the origin, and the type cannot tell it from one that does not:
  * Tensors with two lower indices, e.g. an inertia about a point or a mass matrix, which the parallel axis theorem shifts, rather than a metric.
  * Mixed tensors, e.g. the inertia written as the linear map from the angular velocity to the angular momentum about a point, rather than a velocity gradient.
  * Scalars, e.g. a potential energy `m g z`, which depends on the origin of `z`, or a moment of a force about a point in two dimensions, rather than a mass.

  So does a proper rigid transformation: its translation rejects tensors with two lower indices and tensors with an upper and then a lower index after the rotation, while scalars and tensors with a lower and then an upper index already fail at the rotation, which, like the other linear transformations, does not transform them ("'TransformationTraits' trait not specialized for given type.").
  The rule is conservative: it errs on the side of rejecting a tensor that a translation would leave unchanged, which the user can then transform by the rotation alone, rather than silently leaving a quantity about the origin unchanged.
* A tensor with two upper indices that is about a point, e.g. the second moment `Σ m p p^T` of points about the origin, is left unchanged too, which is wrong for it: it is computed from the points after transforming them.
  Likewise, a moment about a point, e.g. a torque `r × F`, is a vector, which translations leave unchanged (see [Moving frames](#moving-frames)).

### Metric

`dotProduct`, `norm` and `squaredNorm` computed `sum_i v_i w_i`, `crossProduct` the cross product of the components, and `transpose()` of a vector or a covector lowered or raised its index while keeping the components: all of them silently assumed the Euclidean metric in an orthonormal basis.
A dot product needs a metric, `g(v, w) = g_ij v^i w^j`, which Carroll calls the inner product or dot product ([Lecture Notes on General Relativity](https://arxiv.org/abs/gr-qc/9712019), eq. 1.55), and the Euclidean formula is only right in orthonormal bases of a Euclidean space, under orthogonal transformations: the Euclidean "dot product" of a spacetime vector changed from 1010100 to 1249730 m² under a boost, while the Minkowski interval `eta_ij v^i v^j` was preserved.
The pairing `w_i v^i` of a covector and a vector needs no metric (Carroll, eq. 1.35, and the text after eq. 1.63: the action of the gradient on vectors "is perfectly well defined regardless of any metric").
`SpacetimeVector` was an alias of `PositionVector`, expressed in a coordinate system that nothing marked as Minkowski.

Decision:

* A coordinate system has a metric, its second template parameter: `Euclidean` by default, or `Minkowski`, e.g. `CoordinateSystem<Spacetime, Minkowski>`, which the trait `Metric<CoordinateSystem>` returns.
  The metric is part of the type, so coordinate systems with the same identifier and different metrics do not match, and it needs no specialization of a trait by the user, which every translation unit would have to see before the coordinate system is used.
* `dotProduct`, `norm`, `squaredNorm`, `crossProduct` and `transpose()` of a vector or a covector require a Euclidean coordinate system, and fail with "dotProduct, norm, squaredNorm, crossProduct and the transpose of a vector or a covector use the Euclidean metric, which this coordinate system does not have. ...", which points to `innerProduct` and `getInvariant()`.
  The Euclidean metric in an orthonormal basis is the identity, so the components of a vector and of the covector it lowers to are equal, and these operations compute with the components alone: "in Euclidean space (where the metric is diagonal with all entries +1) a dual vector is turned into a vector with precisely the same components when we raise its index" (Carroll, after eq. 1.62), while "in a Lorentzian spacetime the components are not equal" (eq. 1.63).
  Tensors of rank 0 and 2 need no metric for them: the norm of a scalar is its absolute value, and the transpose of a tensor of rank 2 swaps its indices.
* `Minkowski` is the metric `eta = diag(1, -1, ..., -1)` with the signature `(+, -, ..., -)` and the time component first, in length units (`x^0 = c t`), like `LorentzTransformation`.
  In a Minkowski coordinate system, `innerProduct` pairs a covector with a vector, the metric lowers an index explicitly, `innerProduct(eta, v)` for the metric as a tensor with two lower indices, and `getInvariant(v)` computes the interval `eta_ij v^i v^j`, which Lorentz transformations preserve (`tests/src/LorentzTransformation.cpp`).
  `getInvariant()` requires a Minkowski coordinate system, and `SpacetimeVector` and `SpacetimePoint` are constrained to one (the concept `MinkowskiCoordinateSystem`).
* The transformations whose definition needs a metric are constrained to it, so that e.g. a Euclidean rotation of a spacetime vector, or a Lorentz transformation of a vector in space, does not compile: `LorentzTransformation`, which preserves the Minkowski metric, transforms between Minkowski coordinate systems (`MinkowskiCoordinateSystem`), and `ActiveSpatialRotation`, `PassiveSpatialRotation` and the proper rigid transformations, whose rotations are orthogonal with respect to the Euclidean metric, `R R^T = I`, between Euclidean ones (`EuclideanCoordinateSystem`).
  A spatial rotation of spacetime is a `LorentzTransformation`.
  Linear transformations, Jacobians and translations need no metric, and take every coordinate system.
* The metric is a constant of the coordinate system, the identity for orthonormal axes and `eta` for an inertial frame.
  Other metrics are out of scope: those of axes that are not orthonormal, e.g. `A^-T A^-1` in `To` of a passive linear transformation `A` that is not orthogonal, e.g. a scaling, from a Euclidean coordinate system, and those of curvilinear coordinates, which depend on the position, e.g. `diag(1, r^2)` of polar coordinates.
  Such coordinate systems are Euclidean by default, so their dot product is wrong, which *wuzp* does not detect; the components of curvilinear coordinates usually have different units, e.g. `m` and `rad`, which rejects it anyway.
  Jacobians map between curvilinear and Cartesian coordinates without a metric (see [Jacobians](#jacobians)).

### Cross product

The cross product of two vectors is not a vector but a pseudovector, more precisely a covector density: `(a × b)_i = ε_ijk a^j b^k` contains the Levi-Civita symbol, a tensor density, which "explains why the cross product changes sign under parity" (Carroll, eq. 1.90, and eqs. 2.38 to 2.41 for densities), and an invertible linear transformation `M` maps it to `(M a) × (M b) = det(M) M^-T (a × b)` ([Cross product](https://en.wikipedia.org/wiki/Cross_product#Algebraic_properties)), which is `M (a × b)` only for a proper rotation, `det(M) = 1` and `M^-T = M`.
A reflection flips its sign, and a scaling or a shear does not transform it like a vector (`tests/src/ActiveLinearTransformation.cpp`).

Decision: `crossProduct` returns a vector, which is right in right-handed orthonormal bases under proper rotations, the transformations of rigid bodies, whose rotations check `det(R) > 0` (see [Validity of rotations and Lorentz transformations](#validity-of-rotations-and-lorentz-transformations)).
It requires a Euclidean coordinate system (see [Metric](#metric)).
A linear transformation that is not a rotation transforms the result like a vector, which is wrong for it: compute the cross product of the transformed vectors instead.
The same holds for the quantities defined by cross products, e.g. angular velocities, torques and angular momenta.
Typing them as densities would need tensor densities, which *wuzp* does not model.

### Forces and torques

The power of a force along a velocity is their pairing `f · v`, so forces are covectors, the duals of velocities, like the spatial force vectors of Featherstone: "this scalar product makes F6 the dual vector space of M6", the space of motion vectors, and "the power delivered by that force is power = v . f" ([A Short Course on Spatial Vector Algebra](http://royfeatherstone.org/teaching/2008/slides.pdf), slides 3 and 31, and its [notes](http://royfeatherstone.org/teaching/2008/notes.pdf); Featherstone, Rigid Body Dynamics Algorithms, Springer, 2008).
A Jacobian pulls them back, `τ = J^T F` (see [Jacobians](#jacobians)).
Robotics code usually treats forces and torques as vectors, though, and in a Euclidean coordinate system with orthonormal axes, the components of a vector and of its covector are equal.

Decision: `ForceVector` and `TorqueVector` remain vectors, for the Euclidean use, and `ForceCovector` is the covector, e.g. for pullbacks.
`transpose()` turns one into the other in a Euclidean coordinate system (see [Metric](#metric)), e.g. `jacobian.pullback(force.transpose())` for a `ForceVector`.

### Moving frames

"Frame" means three different things, which this document used to mix.
Drake's [multibody notation](https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html) keeps them apart: its [spatial vectors](https://drake.mit.edu/doxygen_cxx/group__multibody__spatial__vectors.html) "must specify both a point and an expressed-in frame. Motion quantities must also state the reference frame with respect to which the motion is measured."

1. The basis a quantity is expressed in, i.e. what its components refer to, Drake's expressed-in frame `W` of `w_AB_W`, the angular velocity of `B` measured in `A`, expressed in `W`: *wuzp*'s coordinate systems, together with an origin for points.
   A passive transformation changes it.
   "A physical quantity does not have an expressed-in frame; that is necessary only for numerical computation" ([Notation Basics](https://drake.mit.edu/doxygen_cxx/group__multibody__notation__basics.html)).
2. The frame a quantity is measured in, for velocities and accelerations: the velocity of a point measured in a moving frame differs from its velocity measured in a fixed frame by more than a change of basis, `v = v_rel + ω × r + v_o`, and its acceleration by the transport terms `a = a_rel + α × r + ω × (ω × r) + 2 ω × v_rel + a_o`, with the centripetal and the Coriolis acceleration, e.g. Drake's [`SpatialVelocity::ComposeWithMovingFrameVelocity()`](https://drake.mit.edu/doxygen_cxx/classdrake_1_1multibody_1_1_spatial_velocity.html), `v_MCo = v_MBo + ω_MB × p_BoCo + v_BCo`, and [`SpatialAcceleration::ComposeWithMovingFrameAcceleration()`](https://drake.mit.edu/doxygen_cxx/classdrake_1_1multibody_1_1_spatial_acceleration.html).
3. The point a quantity is about: a spatial velocity (twist) of a rigid body shifts from a point `Bo` to a point `Co` as `v_MCo = v_MBo + ω_MB × p_BoCo`, a spatial force (wrench) from `Bp` to `Bq` as `τ_Bq = τ_Bp - p_BpBq × f`, e.g. Drake's `SpatialVelocity::Shift()` and [`SpatialForce::Shift()`](https://drake.mit.edu/doxygen_cxx/classdrake_1_1multibody_1_1_spatial_force.html), and an inertia by the parallel axis theorem; in Lynch and Park, Modern Robotics, Cambridge University Press, 2017, the adjoint map of a rigid transformation shifts and rotates twists and wrenches (sections 3.3.2 and 3.4).

Decision: *wuzp* handles the first, and for points the origin.
The other two need state that *wuzp* does not track, the motion of the frames and the points the quantities are about: a passive transformation of a velocity re-expresses it, without adding `ω × r` or the velocity of the frame, and a rigid transformation of a torque ignores the moment arm `p × f`.
The transport formulas are written out where they are needed, e.g. in `tests/src/RigidBodyKinematics.cpp`.
A quantity that depends on the origin or on a point, e.g. a moment about a point or an inertia about a point, must not be a relative tensor that translations leave unchanged, which is the rule of [Translations of tensors](#translations-of-tensors): they reject tensors with two lower indices and mixed tensors, but a moment `r × F` is a vector, which they leave unchanged, so the user shifts it.
Twist and wrench types that track the measured-in frame and the reference point are out of scope for 1.0.

### Approximate comparison

`isApprox` compares relatively, like Eigen's [`isApprox`](https://eigen.tuxfamily.org/dox/classEigen_1_1DenseBase.html#ae8443357b808cd393be1b51974213f9c): `||a - b|| <= p min(||a||, ||b||)`, in the Frobenius norm of the values, with every value matrix backend.
Eigen warns that "because of the multiplicativeness of this comparison, one can't use this function to check whether *this is approximately equal to the zero matrix or vector".

Decision: it stays relative, which suits values of any magnitude, and its consequences are documented on `Tensor::isApprox()`:

* It cannot compare with zero: only zero is approximately equal to zero.
* For points, the components depend on the origin, and so does the comparison: two points 1 mm apart are approximately equal with the precision `1e-6` at 10 km from the origin, but not at 1 m.
  The distance of points, the norm of their difference, does not depend on the origin: compare it with an absolute tolerance, e.g. `(p - q).norm().getValue() <= 1e-9`.
* The Frobenius norm sums the squares of all values, also of components with different units, e.g. `m` and `rad`, so the comparison of such tensors depends on the units.

There is no helper for the absolute comparison: quantities have no ordering, which a helper that checks the unit of the tolerance would need, and the expression above is one line.

### Powers of negative quantities

`pow<P, Q>()` computed the power of the value with `std::pow`, for which "if base is finite and negative and exp is finite and non-integer, a domain error occurs", and which returns NaN then ([`std::pow`](https://en.cppreference.com/w/cpp/numeric/math/pow#Error_handling)), also if the reduced denominator `Q` of the exponent is odd: `pow<1, 3>()` of `-8 m³` was NaN, not `-2 m`, which the standard library computes with `std::cbrt` instead.
With an integer value type, the NaN was converted into the value type, which is undefined behavior.

Decision: a negative value has a real power if `Q` is odd, `(-1)^P |x|^(P/Q)`, e.g. `-2 m` for `pow<1, 3>()` of `-8 m³` and `4 m²` for `pow<2, 3>()`, which `pow()` computes for every value type.
If `Q` is even, a negative value has no real power: floating point values give NaN, like `std::pow`, and integer values throw `std::domain_error`, since a NaN cannot be converted into an integer (`tests/src/Quantity.cpp`).
Like the other powers of integer values, the real roots are computed in floating point and truncated (see [Integer values](#integer-values)).

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

None: the questions of the review are decided above.

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
