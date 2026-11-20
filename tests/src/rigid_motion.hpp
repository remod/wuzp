#pragma once

#include <utility>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using TimeQuantity = QuantityD<Time>;

/**
 * A rigid motion of the coordinate system From relative to the coordinate system To, e.g. of a body, whose derivatives are known in
 * closed form: From rotates about an axis n fixed in To by the angle theta(t) = w0 t + alpha t^2 / 2, R(t) = Exp(n theta(t)) R(0),
 * so that its angular velocity is n (w0 + alpha t) and its angular acceleration n alpha, both expressed in To, and its origin moves
 * with a constant acceleration a, p(t) = p(0) + v(0) t + a t^2 / 2. The tests check the transport formulas and the spatial vectors
 * against central differences of its poses X_ToFrom(t) = (R(t), p(t)).
 */
template <typename FromT_, typename ToT_>
class RigidMotion {
 public:
  using RotationT = wuzp::PassiveSpatialRotation3<ValueMatrix, double, FromT_, ToT_>;
  using RigidT = wuzp::PassiveProperRigidTransformation3<ValueMatrix, FromT_, ToT_, double>;
  using AxisT = wuzp::DirectionVector3<ValueMatrix, double, ToT_>;
  using PositionT = wuzp::PositionVector3<ValueMatrix, double, ToT_>;
  using VelocityT = wuzp::VelocityVector3<ValueMatrix, double, ToT_>;
  using AccelerationT = wuzp::AccelerationVector3<ValueMatrix, double, ToT_>;
  using AngularVelocityT = wuzp::AngularVelocityVector3<ValueMatrix, double, ToT_>;
  using AngularAccelerationT = wuzp::AngularAccelerationVector3<ValueMatrix, double, ToT_>;

  RigidMotion(RotationT initialRotation, AxisT axis, QuantityD<AngularVelocity> initialAngularSpeed,
              QuantityD<AngularAcceleration> angularAcceleration, PositionT initialPosition, VelocityT initialVelocity,
              AccelerationT acceleration)
      : initialRotation_{std::move(initialRotation)},
        axis_{std::move(axis)},
        initialAngularSpeed_{std::move(initialAngularSpeed)},
        angularAcceleration_{std::move(angularAcceleration)},
        initialPosition_{std::move(initialPosition)},
        initialVelocity_{std::move(initialVelocity)},
        acceleration_{std::move(acceleration)} {}

  //! The rotation R(t) = Exp(n theta(t)) R(0) from From to To.
  RotationT rotationAt(const TimeQuantity& time) const {
    return initialRotation_.boxPlus(axis_ * (initialAngularSpeed_ * time + angularAcceleration_ * time * time * 0.5));
  }
  //! The position p(t) of the origin of From relative to the origin of To, expressed in To.
  PositionT positionAt(const TimeQuantity& time) const {
    return initialPosition_ + initialVelocity_ * time + acceleration_ * (time * time * 0.5);
  }
  //! The pose X_ToFrom(t) = (R(t), p(t)).
  RigidT poseAt(const TimeQuantity& time) const {
    return RigidT{typename RigidT::Rotation{rotationAt(time).getQuantities()},
                  typename RigidT::Translation{positionAt(time).getQuantities()}};
  }
  //! The angular velocity n (w0 + alpha t) of From relative to To, expressed in To.
  AngularVelocityT angularVelocityAt(const TimeQuantity& time) const {
    return AngularVelocityT{axis_ * (initialAngularSpeed_ + angularAcceleration_ * time)};
  }
  //! The angular acceleration n alpha of From relative to To, expressed in To.
  AngularAccelerationT angularAcceleration() const { return AngularAccelerationT{axis_ * angularAcceleration_}; }
  //! The velocity v(t) of the origin of From relative to To, expressed in To.
  VelocityT velocityAt(const TimeQuantity& time) const { return initialVelocity_ + acceleration_ * time; }
  //! The acceleration a of the origin of From relative to To, expressed in To.
  const AccelerationT& acceleration() const { return acceleration_; }

 private:
  RotationT initialRotation_;
  AxisT axis_;
  QuantityD<AngularVelocity> initialAngularSpeed_;
  QuantityD<AngularAcceleration> angularAcceleration_;
  PositionT initialPosition_;
  VelocityT initialVelocity_;
  AccelerationT acceleration_;
};
