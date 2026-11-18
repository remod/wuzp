# Conventions

This page collects the conventions *wuzp* follows, with their sources.
The [design notes](https://github.com/remod/wuzp/blob/master/DESIGN.md) explain the decisions behind them.

## Index placement

*wuzp* follows chapter 1 of Carroll's [Lecture Notes on General Relativity](https://arxiv.org/abs/gr-qc/9712019), on which his book Spacetime and Geometry is based:

* An upper index is contravariant, e.g. the components `v^i` of a vector, a lower index covariant, e.g. the components `w_i` of a covector (eqs. 1.30 to 1.39).
* Vectors are stored as columns, covectors as rows (eq. 1.39).
* The first index of a tensor of rank 2 is the row of its matrix, the second the column, like `M^i_j` in eq. 1.54.
* A contraction pairs an upper with a lower index, and the order of the indices matters (eqs. 1.59 and 1.60): `innerProduct(lhs, rhs)` contracts the last index of `lhs` with the first index of `rhs`.
* The pairing `w_i v^i` of a covector and a vector needs no metric (eq. 1.35); the dot product `g_ij v^i w^j` does (eq. 1.55).
* The inverse of a tensor of rank 2 maps the indices `(X, Y)` to `(not Y, not X)`, e.g. `g_ij` to `g^ij`, with `g^ij g_jk = δ^i_k` (eq. 1.56).

## Transformation rules

For a linear transformation with component matrix `A`, i.e. contravariant components transform as `v' = A v`:

| Object | Indices | Rule |
| --- | --- | --- |
| Vector | `Indices<Up>` | `v' = A v` |
| Point | `Indices<Up>`, absolute | `p' = A p` (via a relative intermediate) |
| Covector | `Indices<Down>` | `w' = w A^-1` |
| Linear map | `Indices<Up, Down>` | `T' = A T A^-1` |
| Bilinear form, e.g. a mass matrix | `Indices<Down, Down>` | `M' = A^-T M A^-1` |
| E.g. a covariance matrix | `Indices<Up, Up>` | `T' = A T A^T` |

`A^-1` appears twice in the rule for bilinear forms because `A` transforms components, so covariant indices transform with `A^-1`.
Equivalently, `A^-1` is the change-of-basis matrix `P` of the [Wikipedia article on change of basis](https://en.wikipedia.org/wiki/Change_of_basis) ("old = P new").

The matrix `A` of a passive linear transformation maps the components of a vector in `From` to its components in `To`: its rows are the dual basis covectors of `To`, and the basis vectors of `To` are the columns of `A^-1`, all expressed in `From` (Carroll, eqs. 1.38 and 1.30).
Only for orthonormal bases, e.g. for a rotation, are the rows of `A` also the basis vectors of `To`.

A Jacobian `J` of a change of coordinates from `From` to `To` maps tangent objects, not points: `transform()` pushes vectors forward, `v' = J v`, and tensors with two upper indices, `T' = J T J^T`; `pullback()` pulls covectors back, `w' = w J`, and tensors with two lower indices, `M' = J^T M J` (Lee, Introduction to Smooth Manifolds, 2nd ed., Springer, 2013, chapters 3, 11 and 12).

## Active and passive transformations

* An active transformation moves the objects expressed in its coordinate system.
* A passive transformation from `From` to `To` expresses the objects of `From` in `To`, and only takes objects expressed in `From` ([Active and passive transformation](https://en.wikipedia.org/wiki/Active_and_passive_transformation)).
* `PassiveTransformation<From = B, To = A>` is `X_AB` in the [monogram notation of Drake](https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html) and `^A_B T` in the notation of Craig (Introduction to Robotics, chapter 2): it maps the coordinates of a point in `B` to its coordinates in `A`.
* Passive transformations compose like Drake's, `X_AB * X_BC = X_AC`, i.e. `^A_C T = ^A_B T ^B_C T` in the notation of Craig, and the coordinate systems that meet must match.
* An active transformation and the passive transformation with the same parameters cancel each other: the passive rotation by the angle of an active rotation `R` has the matrix `R^T`, the passive translation by `t` the vector `-t`, and the passive proper rigid transformation with the parameters `(R, t)` of a motion `(R^T, -R^T t)`.

## Translations and rigid transformations

* The vector of a passive translation is the vector from the origin of `To` to the origin of `From`, expressed in `To`, i.e. Drake's `p_AoBo_A`.
* A proper rigid transformation first rotates, then translates: `x -> R x + t`.
* In a passive proper rigid transformation, the rotation transforms from `From` into an intermediate coordinate system with the origin of `From` and the axes of `To`, and the translation from there into `To`, so the translation is expressed in `To`, like that of Drake's [RigidTransform](https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rigid_transform.html) `X_AB = (R_AB, p_AoBo_A)`.
* Rigid transformations compose as `(R1, t1) * (R2, t2) = (R1 R2, R1 t2 + t1)` and invert as `(R, t)^-1 = (R^T, -R^T t)` (Solà, Deray and Atchuthan, ["A micro Lie theory for state estimation in robotics"](https://arxiv.org/abs/1812.01537), eqs. 170 and 171).
* Translations leave vectors, covectors and tensors with two upper indices unchanged, move points, and reject the other tensors, which may depend on the origin.

## Metrics and spacetime

* The default metric of a coordinate system is Euclidean, the identity of orthonormal axes, where a vector and the covector it lowers to have the same components (Carroll, after eq. 1.62).
* `Minkowski` is `η = diag(1, -1, ..., -1)`, with the signature `(+, -, ..., -)` and the time component first, in length units (`x^0 = c t`).
  Carroll uses the signature `(-, +, +, +)`, whose metric is `-η`; both give the same Lorentz transformations, defined by `η = Λ^T η Λ` (eq. 1.14).
* The passive Lorentz boost has the standard form with `Λ_0i = -γ β_i`, for the velocity of `To` relative to `From`, expressed in `From`.
* The inverse of a Lorentz transformation is `η Λ^T η`, which follows from `η = Λ^T η Λ`; that of a rotation is `R^T`.

## Units

* Angles are dimensionless, with every units backend, like the SI treats them ([SI Brochure](https://www.bipm.org/en/publications/si-brochure), 9th edition, section 2.3.3).
* The units of a matrix that describes a linear map have the form `M_ij = a_i / b_j`, and those of its inverse are `(M^-1)_ij = b_i / a_j` (Hart, [Multidimensional Analysis](https://doi.org/10.1007/978-1-4612-4208-6), 1995, sections 3.1 and 3.3).
* The units of a Jacobian are `[x_i] / [q_j]` for the coordinates `q` of `From` and `x` of `To`.

## Comparisons

`isApprox(other, precision)` compares relatively, like Eigen's [`isApprox`](https://eigen.tuxfamily.org/dox/classEigen_1_1DenseBase.html#ae8443357b808cd393be1b51974213f9c): `||a - b|| <= p min(||a||, ||b||)`, in the Frobenius norm of the values.
It cannot compare with zero, and for points it depends on the origin (see [Limitations](limitations.md)).
