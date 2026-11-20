# Coordinate systems and metrics

Every tensor is expressed in a coordinate system, a type named after an identifier:

```cpp
struct Base {};
struct Camera {};
using BaseFrame = wuzp::CoordinateSystem<Base>;
using CameraFrame = wuzp::CoordinateSystem<Camera>;
```

Operations that combine tensors require them to be expressed in the same coordinate system, and the error says so: adding a force in `BaseFrame` to a force in `CameraFrame` fails with "Coordinate systems do not match.".
A transformation expresses a tensor in another coordinate system (see [Transformations](transformations.md)).
The coordinate system is a type, so it has no runtime cost, and it is fixed at compile time.

A coordinate system means axes, an origin and a metric.
It is not a moving observer: a passive transformation of a velocity re-expresses it, without adding the velocity of the frame (see [Limitations](../limitations.md)).
Coordinate systems also name the frames and the points of the spatial vectors, whose types record the frame a motion is measured in, the frame it describes and the point it is about: a point is the origin of a coordinate system, e.g. `using Tip = wuzp::CoordinateSystem<TipId>;` (see [Spatial vectors](spatial-vectors.md)).

`castCoordinateSystem<To>(tensor)` reinterprets the components of a tensor in another coordinate system without transforming them, e.g. for a block of a larger vector whose components are those of a coordinate system of its own.

## Metrics

A dot product needs a metric, `g(v, w) = g_ij v^i w^j` ([Carroll, Lecture Notes on General Relativity](https://arxiv.org/abs/gr-qc/9712019), eq. 1.55), so a coordinate system has one, its second template parameter:

* `wuzp::Euclidean`, the default, the metric `δ_ij` of orthonormal axes, e.g. of space in Cartesian coordinates.
* `wuzp::Minkowski`, the metric `η = diag(1, -1, ..., -1)` of an inertial frame of special relativity, with the signature `(+, -, ..., -)` and the time component first, in length units (`x^0 = c t`).

```cpp
struct Lab {};
using Spacetime = wuzp::CoordinateSystem<Lab, wuzp::Minkowski>;
```

The metric is part of the type, so coordinate systems with the same identifier and different metrics do not match.

`dotProduct`, `norm`, `squaredNorm`, `crossProduct` and the transpose of a vector or a covector compute with the components alone, which is right for the Euclidean metric in an orthonormal basis, where a vector and the covector it lowers to have the same components (Carroll, after eq. 1.62).
They require a Euclidean coordinate system, and fail otherwise with a message that points to the operations that need no metric or use the Minkowski metric:

```cpp
using Displacement = wuzp::SpacetimeVector4<wuzp_eigen::ValueMatrix, double, Spacetime>;
const Displacement displacement{1.0, 2.0, 3.0, 4.0};
// displacement.norm();             // Error: ... use the Euclidean metric, which this coordinate system does not have.
const auto interval = getInvariant(displacement);  // OK: (c t)^2 - x^2 - y^2 - z^2, in m^2.
```

* `innerProduct(covector, vector)`, the pairing `w_i v^i`, needs no metric: "the action of the gradient on vectors is perfectly well defined regardless of any metric" (Carroll, after eq. 1.63).
* `getInvariant(v)` is the interval `η_ij v^i v^j` of a vector in a Minkowski coordinate system, which Lorentz transformations preserve.

The transformations whose definition needs a metric are constrained to it: rotations and proper rigid transformations, which are orthogonal, `R R^T = I`, take Euclidean coordinate systems, and Lorentz transformations, which preserve `η`, Minkowski ones.
Linear transformations, Jacobians and translations take every coordinate system.

## What is not modeled

The metric is a constant of the coordinate system.
Other metrics, e.g. of axes that are not orthonormal or of curvilinear coordinates such as polar coordinates, whose metric depends on the position, are not modeled: such coordinate systems are Euclidean by default, so their dot product is wrong, which *wuzp* does not detect.
The components of curvilinear coordinates usually have different units, e.g. `m` and `rad`, which rejects the dot product anyway, and Jacobians map between curvilinear and Cartesian coordinates without a metric (see [Transformations](transformations.md#jacobians)).
See the [design notes](https://github.com/remod/wuzp/blob/master/DESIGN.md#metric) for details.
