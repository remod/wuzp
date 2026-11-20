# Rigid body kinematics

The velocity and the acceleration of a point `Q` fixed to a rigid body follow from the velocity `v_P` and the acceleration `a_P` of another point `P` of the body, its angular velocity `ω` and angular acceleration `α`, and the position vector `p_PQ` from `P` to `Q`, all measured in an inertial frame and expressed in one coordinate system, like Drake's [`SpatialVelocity::Shift()`](https://drake.mit.edu/doxygen_cxx/classdrake_1_1multibody_1_1_spatial_velocity.html) and [`SpatialAcceleration::Shift()`](https://drake.mit.edu/doxygen_cxx/classdrake_1_1multibody_1_1_spatial_acceleration.html) shift them from one point of a body to another:

```
v_Q = v_P + ω × p_PQ
a_Q = a_P + α × p_PQ + ω × (ω × p_PQ)
```

`velocityOfBodyFixedPoint()` and `accelerationOfBodyFixedPoint()` compute them, and check the units and the coordinate system of each vector: e.g. swapped arguments fail with "Units do not match.".
Angles are dimensionless, so `ω` is in `1/s` and `ω × p_PQ` in `m/s` without dividing by a radian (see [Units and quantities](../concepts/units.md)).
The spatial vectors also check the frames and the points: a spatial velocity `V_ABp_A` of the body `B` in the inertial frame `A` about `P` shifts to `V_ABq_A` about `Q` (see [Spatial vectors](../concepts/spatial-vectors.md)).
This is the test `tests/src/RigidBodyKinematics.cpp`, written for the Eigen backend and *wuzp*'s own units:

```cpp
#include <wuzp/wuzp.hpp>
#include <wuzp_eigen/ValueMatrix.hpp>
#include <wuzp_units/Unit.hpp>

using wuzp_eigen::ValueMatrix;

struct Inertial {};
struct Body {};
struct P {};
struct Q {};
using InertialFrame = wuzp::CoordinateSystem<Inertial>;
using BodyFrame = wuzp::CoordinateSystem<Body>;
using PointP = wuzp::CoordinateSystem<P>;  // A point is the origin of a coordinate system.
using PointQ = wuzp::CoordinateSystem<Q>;

using Position = wuzp::PositionVector3<ValueMatrix, double, InertialFrame>;
using Velocity = wuzp::VelocityVector3<ValueMatrix, double, InertialFrame>;
using Acceleration = wuzp::AccelerationVector3<ValueMatrix, double, InertialFrame>;
using AngularVelocity = wuzp::AngularVelocityVector3<ValueMatrix, double, InertialFrame>;
using AngularAcceleration = wuzp::AngularAccelerationVector3<ValueMatrix, double, InertialFrame>;
template <typename PointT>
using SpatialVelocityOfBody = wuzp::SpatialVelocity<ValueMatrix, double, InertialFrame, BodyFrame, PointT, InertialFrame>;
template <typename PointT>
using SpatialAccelerationOfBody = wuzp::SpatialAcceleration<ValueMatrix, double, InertialFrame, BodyFrame, PointT, InertialFrame>;

int main() {
  const Velocity velocity{1.0, 2.0, 3.0};
  const Acceleration acceleration{1.0, 2.0, 3.0};
  const AngularVelocity angularVelocity{4.0, 1.0, 2.0};
  const AngularAcceleration angularAcceleration{2.0, -1.0, 3.0};
  const Position p_PQ{0.0, 0.0, 1.0};  // Q relative to P, a vector.

  const Velocity velocityOfQ = wuzp::velocityOfBodyFixedPoint(velocity, angularVelocity, p_PQ);  // (2, -2, 3) m/s
  const Acceleration accelerationOfQ =
      wuzp::accelerationOfBodyFixedPoint(acceleration, angularAcceleration, angularVelocity, p_PQ);  // (8, 2, -14) m/s^2
  // wuzp::velocityOfBodyFixedPoint(velocity, p_PQ, angularVelocity);  // Error: Units do not match.

  const SpatialVelocityOfBody<PointP> V_ABp{angularVelocity, velocity};
  const SpatialVelocityOfBody<PointQ> V_ABq = V_ABp.shift<PointQ>(p_PQ);  // (4, 1, 2) 1/s, (2, -2, 3) m/s
  const SpatialAccelerationOfBody<PointQ> A_ABq =
      SpatialAccelerationOfBody<PointP>{angularAcceleration, acceleration}.shift<PointQ>(p_PQ, V_ABp);  // (8, 2, -14) m/s^2
  // V_ABp + V_ABq;  // Error: Reference points do not match.
  return 0;
}
```

The position vector `p_PQ` is a vector, the difference of two points, not a point: a point would make `ω × p_PQ` depend on the origin of the coordinate system, and the functions reject it with "Absolute/relative mismatch.".

## A point that moves in a rotating frame

A frame `C` that moves relative to the body, e.g. a slider on a rotating rod, has the velocity and the acceleration of the point of the body where its origin is, plus those relative to the body, and the Coriolis acceleration `2 ω_AB × v_BCo`.
`composeWithMovingFrameVelocity()` and `composeWithMovingFrameAcceleration()` compose them, like Drake's `ComposeWithMovingFrameVelocity()` and `ComposeWithMovingFrameAcceleration()`; the spatial velocity and acceleration of `C` are measured in `B`, the frame of the spatial vectors they compose with:

```cpp
struct Slider {};
using SliderFrame = wuzp::CoordinateSystem<Slider>;
// The slider C relative to the body B, about the origin of C, expressed in A.
using VelocityOfSlider = wuzp::SpatialVelocity<ValueMatrix, double, BodyFrame, SliderFrame, SliderFrame, InertialFrame>;
using AccelerationOfSlider = wuzp::SpatialAcceleration<ValueMatrix, double, BodyFrame, SliderFrame, SliderFrame, InertialFrame>;

const Position p_PCo{1.0, 0.0, 0.0};
const VelocityOfSlider V_BCo{AngularVelocity::zero(), Velocity{1.0, 0.0, 0.0}};  // Sliding along the rod.
const AccelerationOfSlider A_BCo{AngularAcceleration::zero(), Acceleration::zero()};
// The slider C measured in A: V_ACo and A_ACo, with the Coriolis acceleration 2 w x v_BCo = (0, 4, -2) m/s^2.
const auto V_ACo = V_ABp.composeWithMovingFrameVelocity(p_PCo, V_BCo);
const auto A_ACo = SpatialAccelerationOfBody<PointP>{angularAcceleration, acceleration}.composeWithMovingFrameAcceleration(
    p_PCo, V_ABp, V_BCo, A_BCo);
```

The test checks these formulas against central differences of the poses `X_AB(t) X_BC(t)` of a body and a frame that both rotate and translate with angular and linear accelerations.

## Expressing the result in another coordinate system

A passive rotation or a passive rigid transformation re-expresses a velocity or a spatial velocity, e.g. in the coordinate system of the body:

```cpp
using RotationToBody = wuzp::PassiveSpatialRotation3<ValueMatrix, double, InertialFrame, BodyFrame>;

const RotationToBody rotation{0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0};  // The body frame is the inertial frame rotated by pi/2 around z.
const wuzp::VelocityVector3<ValueMatrix, double, BodyFrame> velocityInBody = rotation.transform(velocityOfQ);
const auto V_ABq_B = rotation.transform(V_ABq);  // Still measured in the inertial frame, about Q.
```

This changes the basis the velocity is expressed in, not the frame it is measured in: the velocity of `Q` relative to the moving body frame is zero, and `composeWithMovingFrameVelocity()` relates the two.
The adjoint map of a rigid transformation also moves the point of a spatial velocity or a spatial force to another origin (see [Spatial vectors](../concepts/spatial-vectors.md#re-expression-and-the-adjoint-map)).
