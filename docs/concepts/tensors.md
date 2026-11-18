# Tensors and index variance

`wuzp::Tensor<Indices, Quantities, CoordinateSystem, Absolute>` is a tensor of rank 0, 1 or 2: its quantities, expressed in a coordinate system, absolute (a point) or relative.
`Indices` records the variance of each index in its order, following chapter 1 of Carroll's [Lecture Notes on General Relativity](https://arxiv.org/abs/gr-qc/9712019): an upper index is contravariant, a lower index covariant.

| Indices | Tensor | Stored as | Example |
| --- | --- | --- | --- |
| `Indices<>` | scalar | 1x1 matrix | a mass |
| `Indices<Up>` | vector `v^i` | column | a velocity, a point |
| `Indices<Down>` | covector `w_i` | row | a gradient, a force as a covector |
| `Indices<Up, Down>` | linear map `T^i_j` | square matrix | the matrix of a rotation |
| `Indices<Down, Down>` | bilinear form `M_ij` | square matrix | a mass matrix, a metric |
| `Indices<Up, Up>` | `S^ij` | square matrix | a covariance |

The first index of a tensor of rank 2 is the row of its matrix, the second the column, like `M^i_j` in Carroll's eq. 1.54, with vectors as columns and covectors as rows (eq. 1.39).
The aliases cover the common cases, e.g. `Vector<Quantities, CoordinateSystem>`, `Covector`, `Point`, `HomogeneousVector<Dimension, ValueMatrix, Value, Unit, CoordinateSystem>`, `PositionVector3`, `VelocityVector3`, `ForceCovector3` (see the [API reference](../api/index.html)).

## Products

`*` and `/` only take scalars, so that neither the elementwise product nor the matrix product is a silent misunderstanding: a tensor can be multiplied by an arithmetic scalar, a quantity or a tensor of rank 0 in the same coordinate system.
The products of tensors of rank 1 or more have names:

```cpp
// velocity * velocity;                 // Error: A tensor can only be multiplied by a scalar, a quantity or a tensor of rank 0.
// innerProduct(velocity, velocity);    // Error: An inner product contracts an upper with a lower index, ...
const auto power = innerProduct(forceCovector, velocity);  // OK: the pairing F_i v^i, in W.
const auto energy = innerProduct(innerProduct(massMatrix, velocity), velocity) / 2;  // OK: M_ij v^i v^j / 2.
const auto componentwise = cwiseProduct(velocity, velocity);  // OK: quantities, without a coordinate system.
```

* `innerProduct(lhs, rhs)` contracts the last index of `lhs` with the first index of `rhs`, which must be an upper and a lower index (Carroll, eqs. 1.59 and 1.60).
  The only index of a vector or a covector is both its first and its last, so `innerProduct(w, v)` and `innerProduct(v, w)` are both `w_i v^i`, and `innerProduct(massMatrix, v)` is the covector `M_ij v^j`.
  The result keeps the remaining indices in their order.
* `outerProduct(lhs, rhs)` concatenates the indices, e.g. `v^i w_j`; tensors have rank 2 at most.
* `cwiseProduct`, `cwiseQuotient` and `cwiseInverse` compute elementwise, which depends on the basis, so they return quantities without indices or a coordinate system, like [Eigen's arrays](https://eigen.tuxfamily.org/dox/group__TutorialArrayClass.html) compute elementwise while its matrices multiply as matrices.
* `dotProduct`, `crossProduct`, `norm` and `squaredNorm` of vectors need the Euclidean metric (see [Coordinate systems and metrics](coordinate-systems.md#metrics)).

## Transpose, inverse and determinant

* `transpose()` swaps the indices of a tensor of rank 2, `(T^T)_j^i = T^i_j`, and turns a vector into a covector and back, which lowers or raises the index with the Euclidean metric (Carroll, eq. 1.62), so it requires a Euclidean coordinate system.
  E.g. `force.transpose()` turns a `ForceVector3` into a `ForceCovector3`.
* `inverse()` is the matrix inverse of a tensor of rank 0 or 2, which maps the indices `(X, Y)` to `(not Y, not X)`: the inverse of a linear map is a linear map, and that of a metric or a mass matrix `M_ij` has two upper indices `M^ij` (Carroll, eq. 1.56).
  Vectors, covectors and points have no inverse; the elementwise reciprocal is `cwiseInverse()`.
* `determinant()` is defined for scalars and for the mixed tensors `Indices<Up, Down>` and `Indices<Down, Up>`, whose determinant is invariant under changes of basis.
  The determinant of a tensor with two lower or two upper indices is a scalar density, not a scalar (Carroll, eq. 2.41), so it is rejected; `getQuantities().determinant()` computes the determinant of the components.

## Batches

A tensor with an upper index may store a batch of vectors or points, one per column, e.g. a point cloud.
Transformations, sums, differences, comparisons, and products and quotients with scalars accept batches; the operations that are only defined for a single vector, e.g. `innerProduct`, `norm()` and `transpose()`, reject them.

## Cross product

`crossProduct` returns a vector, which is right in right-handed orthonormal bases under proper rotations, the transformations of rigid bodies.
The cross product is a pseudovector: a reflection flips its sign, and a scaling or a shear does not transform it like a vector, so transform its factors instead.
The same holds for quantities defined by cross products, e.g. angular velocities and torques (see the [design notes](https://github.com/remod/wuzp/blob/master/DESIGN.md#cross-product)).
