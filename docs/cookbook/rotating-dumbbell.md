# Rotating dumbbell

A dumbbell of two masses `m1` and `m2` on a massless rod of length `2 r`, which moves in a plane, is a standard example of a mass matrix in generalized coordinates ([Mass matrix: rotating dumbbell](https://en.wikipedia.org/wiki/Mass_matrix#Rotating_dumbbell)).
Its minimal coordinates `q = (x, y, φ)` are the position of the center of the rod and its angle, and the Cartesian positions of the masses are

```
(x1, y1) = (x, y) + r (cos φ, sin φ)
(x2, y2) = (x, y) - r (cos φ, sin φ)
```

The example shows three things *wuzp* checks: heterogeneous units in one matrix, Jacobians between coordinate systems of different dimensions, and the variance of the indices of a mass matrix.
It follows the test [`tests/src/RotatingDumbbell.cpp`](https://github.com/remod/wuzp/blob/master/tests/src/RotatingDumbbell.cpp), here for the Eigen backend and *wuzp*'s own units; the snippets below are parts of one function.

## Coordinates with mixed units

The minimal coordinates have the units `(m, m, rad)`, and their time derivatives `(m/s, m/s, 1/s)`, so they are quantities with a unit per element, expressed in a coordinate system of their own:

```cpp
#include <wuzp/wuzp.hpp>
#include <wuzp_eigen/ValueMatrix.hpp>
#include <wuzp_units/Unit.hpp>

using namespace wuzp_units;
using wuzp_eigen::QuantityD;
using wuzp_eigen::ValueMatrix;

struct Minimal {};
using MinimalFrame = wuzp::CoordinateSystem<Minimal>;  // (x, y, φ)
struct Masses {};
using MassesFrame = wuzp::CoordinateSystem<Masses>;  // (x1, y1, x2, y2), in which the mass matrix is diagonal.
struct Cartesian {};
using CartesianFrame = wuzp::CoordinateSystem<Cartesian>;  // (x1, y1), the position of the first mass.

using MinimalCoordinates =
    wuzp::Point<wuzp::Quantities<ValueMatrix<double, 3, 1>, wuzp::TypeMatrix<3, 1, Length, Length, Angle>>, MinimalFrame>;
using MinimalVelocities =
    wuzp::Vector<wuzp::Quantities<ValueMatrix<double, 3, 1>, wuzp::TypeMatrix<3, 1, Velocity, Velocity, AngularVelocity>>, MinimalFrame>;

const QuantityD<Length> radius{2.0};
const QuantityD<Mass> mass1{1.0};
const QuantityD<Mass> mass2{3.0};
const MinimalCoordinates coordinates{0.2, 0.4, 0.5};
const MinimalVelocities velocities{0.7, -0.3, 0.1};
const QuantityD<Angle> angle{coordinates.getQuantity<2>()};
```

The coordinates are a point and their derivatives a vector: the difference of two configurations is a vector, and a Jacobian maps the velocities, not the coordinates.

## The mass matrix

The kinetic energy is `T = q'^T M q' / 2` with the mass matrix

```
    | m1 + m2               0                    -r (m1 - m2) sin φ |
M = | 0                     m1 + m2               r (m1 - m2) cos φ |
    | -r (m1 - m2) sin φ    r (m1 - m2) cos φ     r^2 (m1 + m2)     |
```

whose elements have the units `kg`, `kg m/rad` and `kg m^2/rad^2`, i.e. `kg`, `kg m` and `kg m^2`, since angles are dimensionless.
It is a bilinear form, a tensor with two lower indices, which pairs with two velocities:

```cpp
using LengthPerAngle = wuzp::Division<Length, Angle>::type;
using MassLength = wuzp::Product<Mass, LengthPerAngle>::type;
using MassLength2 = wuzp::Product<MassLength, LengthPerAngle>::type;
using MassMatrixUnits = wuzp::TypeMatrix<3, 3, Mass, Mass, MassLength, Mass, Mass, MassLength, MassLength, MassLength, MassLength2>;
using MassMatrix = wuzp::Tensor<wuzp::Indices<wuzp::Down, wuzp::Down>, wuzp::Quantities<ValueMatrix<double, 3, 3>, MassMatrixUnits>,
                                MinimalFrame, wuzp::Relative>;

const QuantityD<Mass> massSum{mass1 + mass2};
const QuantityD<Mass> noMass{0.0};
const QuantityD<MassLength> coupling0{-radius * (mass1 - mass2) * sin(angle)};
const QuantityD<MassLength> coupling1{radius * (mass1 - mass2) * cos(angle)};
const QuantityD<MassLength2> rotation{pow<2>(radius) * massSum};
const MassMatrix massMatrix{massSum, noMass, coupling0, noMass, massSum, coupling1, coupling0, coupling1, rotation};

// The kinetic energy M_ij q'^i q'^j / 2: each inner product contracts a lower index of M with the upper index of q'.
const QuantityD<Energy> energy{0.5 * innerProduct(innerProduct(massMatrix, velocities), velocities).getQuantity()};
```

`innerProduct(massMatrix, massMatrix)` does not compile, since it would contract two lower indices, and neither does a mass matrix whose units are not consistent with the coordinates.

## The mass matrix is a pullback

The Jacobian `J = d(x1, y1, x2, y2) / d(x, y, φ)` maps from the 3 minimal coordinates to the 4 Cartesian coordinates of the masses.
Its elements have the units `[x_i] / [q_j]`: `1` for the derivatives by `x` and `y`, and `m/rad` for those by `φ`:

```cpp
using MassesJacobian = wuzp::Jacobian<ValueMatrix, double, MinimalFrame, MassesFrame, 3, 4,
                                      Dimensionless, Dimensionless, LengthPerAngle,
                                      Dimensionless, Dimensionless, LengthPerAngle,
                                      Dimensionless, Dimensionless, LengthPerAngle,
                                      Dimensionless, Dimensionless, LengthPerAngle>;
using MassesMassMatrix = wuzp::Tensor<wuzp::Indices<wuzp::Down, wuzp::Down>,
                                      wuzp::Quantities<ValueMatrix<double, 4, 4>, wuzp::TypeMatrix<4, 4, Mass>>, MassesFrame, wuzp::Relative>;

const QuantityD<Dimensionless> one{1.0};
const QuantityD<Dimensionless> zero{0.0};
const QuantityD<LengthPerAngle> dx1dphi{-radius * sin(angle)};
const QuantityD<LengthPerAngle> dy1dphi{radius * cos(angle)};
const MassesJacobian jacobian{one, zero, dx1dphi, zero, one, dy1dphi, one, zero, -dx1dphi, zero, one, -dy1dphi};

const MassesMassMatrix cartesianMassMatrix{mass1,  noMass, noMass, noMass, noMass, mass1,  noMass, noMass,
                                           noMass, noMass, mass2,  noMass, noMass, noMass, noMass, mass2};
const MassMatrix pulledBack = jacobian.pullback(cartesianMassMatrix);  // J^T diag(m1, m1, m2, m2) J, equal to massMatrix.
const wuzp::VelocityVector<4, ValueMatrix, double, MassesFrame> cartesianVelocities = jacobian.transform(velocities);  // J q'
```

The pullback `J^T M_x J` of the diagonal mass matrix of the Cartesian coordinates is the mass matrix of the minimal coordinates, since the kinetic energy is `(J q')^T M_x (J q') / 2`.
The units of the result follow from the units of the Jacobian: `kg m/rad` for the couplings and `kg m^2/rad^2` for the rotation.
`jacobian.transform(cartesianMassMatrix)` does not compile: a Jacobian pushes forward vectors and tensors with two upper indices, and pulls back covectors and tensors with two lower indices.

## The operational space inertia

The inertia that a force at the first mass feels is the operational space inertia of Khatib, `Λ = (J M^-1 J^T)^-1` for the Jacobian `J` of the first mass ("A unified approach for motion and force control of robot manipulators: The operational space formulation", IEEE Journal on Robotics and Automation 3(1), 1987, [doi:10.1109/JRA.1987.1087068](https://doi.org/10.1109/JRA.1987.1087068)).
It needs no operation of its own: the inverse of the mass matrix has two upper indices, so the Jacobian pushes it forward, and the inverse of that has two lower indices again:

```cpp
using FirstMassJacobian = wuzp::Jacobian<ValueMatrix, double, MinimalFrame, CartesianFrame, 3, 2, Dimensionless, Dimensionless,
                                         LengthPerAngle, Dimensionless, Dimensionless, LengthPerAngle>;
using CartesianMassMatrix = wuzp::Tensor<wuzp::Indices<wuzp::Down, wuzp::Down>,
                                         wuzp::Quantities<ValueMatrix<double, 2, 2>, wuzp::TypeMatrix<2, 2, Mass>>, CartesianFrame, wuzp::Relative>;

const FirstMassJacobian firstMassJacobian{jacobian.getQuantities().block<0, 0, 2, 3>()};
const CartesianMassMatrix operationalSpaceInertia{firstMassJacobian.transform(massMatrix.inverse()).inverse()};
```

The rod has no mass, so it only transmits forces along itself: the test checks the result against the closed form `(m1 + m2) u u^T + m1 n n^T`, for the direction `u` of the rod and its normal `n`.
