# Rigid body kinematics

The velocity and the acceleration of a point fixed to a rigid body follow from the velocity `v` and acceleration `a` of a reference point of the body, its angular velocity `ω` and angular acceleration `α`, and the position `r` of the point relative to the reference point, all measured in an inertial frame and expressed in one coordinate system, e.g. like Drake's [`SpatialVelocity::Shift()`](https://drake.mit.edu/doxygen_cxx/classdrake_1_1multibody_1_1_spatial_velocity.html) and [`SpatialAcceleration::Shift()`](https://drake.mit.edu/doxygen_cxx/classdrake_1_1multibody_1_1_spatial_acceleration.html) shift them from one point of a body to another:

```
v_P = v + ω × r
a_P = a + α × r + ω × (ω × r)
```

With *wuzp*, the units of each term are checked, e.g. `ω × r` must be a velocity, and so is the coordinate system in which each vector is expressed.
Angles are dimensionless, so `ω` is in `1/s` and `ω × r` in `m/s` without dividing by a radian (see [Units and quantities](../concepts/units.md)).
This is the test `tests/src/RigidBodyKinematics.cpp`, written for the Eigen backend and *wuzp*'s own units:

```cpp
#include <wuzp/wuzp.hpp>
#include <wuzp_eigen/ValueMatrix.hpp>
#include <wuzp_units/Unit.hpp>

using wuzp_eigen::ValueMatrix;

struct Inertial {};
using InertialFrame = wuzp::CoordinateSystem<Inertial>;

using Position = wuzp::PositionVector3<ValueMatrix, double, InertialFrame>;
using Velocity = wuzp::VelocityVector3<ValueMatrix, double, InertialFrame>;
using Acceleration = wuzp::AccelerationVector3<ValueMatrix, double, InertialFrame>;
using AngularVelocity = wuzp::AngularVelocityVector3<ValueMatrix, double, InertialFrame>;
using AngularAcceleration = wuzp::AngularAccelerationVector3<ValueMatrix, double, InertialFrame>;

int main() {
  const Velocity velocity{1.0, 2.0, 3.0};
  const Acceleration acceleration{1.0, 2.0, 3.0};
  const AngularVelocity angularVelocity{4.0, 1.0, 2.0};
  const AngularAcceleration angularAcceleration{2.0, -1.0, 3.0};
  const Position r{0.0, 0.0, 1.0};  // The point relative to the reference point.

  const Velocity velocityOfPoint = velocity + angularVelocity.crossProduct(r);  // (2, -2, 3) m/s
  const Acceleration accelerationOfPoint =
      acceleration + angularAcceleration.crossProduct(r) + angularVelocity.crossProduct(angularVelocity.crossProduct(r));  // (8, 2, -14) m/s^2
  // const Velocity wrong = velocity + angularVelocity;  // Error: Units do not match.
  return 0;
}
```

The relative position `r` is a vector, the difference of two points, not a point: a point would make `ω × r` depend on the origin of the coordinate system, and `crossProduct` rejects it.

## Expressing the result in another coordinate system

A passive rotation re-expresses the velocity, e.g. in the coordinate system of the body:

```cpp
struct Body {};
using BodyFrame = wuzp::CoordinateSystem<Body>;
using RotationToBody = wuzp::PassiveSpatialRotation3<ValueMatrix, double, InertialFrame, BodyFrame>;

const RotationToBody rotation{0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0};  // The body frame is the inertial frame rotated by pi/2 around z.
const wuzp::VelocityVector3<ValueMatrix, double, BodyFrame> velocityInBody = rotation.transform(velocityOfPoint);
```

This changes the basis the velocity is expressed in, not the frame it is measured in: if the body frame moves, its velocity relative to the moving frame needs the transport terms above, which *wuzp* does not add by itself (see [Limitations](../limitations.md)).
