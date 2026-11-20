#pragma once

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"

// The transport formulas of rigid body kinematics: the velocity and the acceleration of a point fixed to a rigid body from those
// of another point of the body, like the shifts of Drake's spatial velocities and accelerations
// (https://drake.mit.edu/doxygen_cxx/group__multibody__spatial__vectors.html). The spatial vectors, which also track the frame the
// motion is measured in and the point it is about, shift with them (see SpatialVelocity::shift() and SpatialAcceleration::shift()).

namespace wuzp {
namespace internal {

//! The three-dimensional vector with the unit UnitT_, with the values and the coordinate system of the tensor TensorT_. The
//! transport formulas convert their arguments into these vectors, so that misuse fails with the messages of the conversions, e.g.
//! swapped arguments with "Units do not match.", a point instead of a position vector with "Absolute/relative mismatch." and a
//! vector in another coordinate system with "Coordinate systems do not match.".
template <typename TensorT_, typename UnitT_>
using KinematicVector3 =
    Tensor<Indices<Up>,
           Quantities<typename Rebind<typename TensorT_::ValuesT, typename TensorT_::ValueT, 3, 1>::type, TypeMatrix<3, 1, UnitT_>>,
           typename TensorT_::CoordinateSystemT, Relative>;

//! The units of the values of the tensor TensorT_.
template <typename TensorT_>
using KinematicUnits = AccessUnits<AccessImplementation<typename TensorT_::ValueT>>;

}  // namespace internal

/**
 * The velocity v_Q = v_P + w x p_PQ of a point Q fixed to a rigid body, from the velocity v_P of a point P fixed to the body, the
 * angular velocity w of the body and the position vector p_PQ from P to Q. The velocities are measured in the same frame, e.g.
 * an inertial one, and all vectors are expressed in the coordinate system of v_P, which must be Euclidean. Like Drake's
 * SpatialVelocity::Shift(), v_MBq = v_MBp + w_MB x p_BpBq
 * (https://drake.mit.edu/doxygen_cxx/classdrake_1_1multibody_1_1_spatial_velocity.html). The angular velocity and the position vector are
 * converted into three-dimensional vectors of an angular velocity and a length in the coordinate system of v_P, so that e.g. swapped
 * arguments fail with "Units do not match." and a point instead of the position vector with "Absolute/relative mismatch.": p_PQ is the
 * difference of two points.
 */
template <AnyTensor VelocityT_, AnyTensor AngularVelocityT_, AnyTensor OffsetT_>
auto velocityOfBodyFixedPoint(const VelocityT_& velocity, const AngularVelocityT_& angularVelocity, const OffsetT_& offset) {
  using UnitsT = internal::KinematicUnits<VelocityT_>;
  const internal::KinematicVector3<VelocityT_, typename UnitsT::AngularVelocity> w{angularVelocity};
  return velocity + w.crossProduct(internal::KinematicVector3<VelocityT_, typename UnitsT::Length>{offset});
}

/**
 * The acceleration a_Q = a_P + alpha x p_PQ + w x (w x p_PQ) of a point Q fixed to a rigid body, from the acceleration a_P of a
 * point P fixed to the body, the angular acceleration alpha and the angular velocity w of the body and the position vector p_PQ
 * from P to Q, with the tangential acceleration alpha x p_PQ and the centripetal acceleration w x (w x p_PQ). The accelerations
 * and the angular velocity are measured in the same frame, and all vectors are expressed in the coordinate system of a_P, which
 * must be Euclidean. Like Drake's SpatialAcceleration::Shift(), a_MBq = a_MBp + alpha_MB x p_BpBq + w_MB x (w_MB x p_BpBq)
 * (https://drake.mit.edu/doxygen_cxx/classdrake_1_1multibody_1_1_spatial_acceleration.html). The arguments are converted like
 * those of velocityOfBodyFixedPoint().
 */
template <AnyTensor AccelerationT_, AnyTensor AngularAccelerationT_, AnyTensor AngularVelocityT_, AnyTensor OffsetT_>
auto accelerationOfBodyFixedPoint(const AccelerationT_& acceleration, const AngularAccelerationT_& angularAcceleration,
                                  const AngularVelocityT_& angularVelocity, const OffsetT_& offset) {
  using UnitsT = internal::KinematicUnits<AccelerationT_>;
  const internal::KinematicVector3<AccelerationT_, typename UnitsT::AngularAcceleration> alpha{angularAcceleration};
  const internal::KinematicVector3<AccelerationT_, typename UnitsT::AngularVelocity> w{angularVelocity};
  const internal::KinematicVector3<AccelerationT_, typename UnitsT::Length> p{offset};
  return acceleration + alpha.crossProduct(p) + w.crossProduct(w.crossProduct(p));
}

}  // namespace wuzp
