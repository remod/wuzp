#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/SpatialAcceleration.hpp"
#include "wuzp/trafo/SpatialVelocity.hpp"
#include "wuzp/trafo/kinematics.hpp"

#include "central_difference.hpp"
#include "coordinate_systems.hpp"
#include "rigid_motion.hpp"
#include "spatial_vector_types.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

// The transport formulas of rigid body kinematics (#76): the velocity and the acceleration of a point fixed to a rigid body, and of
// a frame that moves relative to the body, checked by hand and against central differences of rigid motions. A is the inertial
// frame, B the body, P and Q are points fixed to B, and C is a frame that moves relative to B.

namespace {

using PointInB = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
using PointInC = PositionPoint3<ValueMatrix, double, CoordinateSystemC>;
using SpatialVelocityABpA = SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA>;
using SpatialVelocityABqA = SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointQ, CoordinateSystemA>;
using SpatialAccelerationABpA = SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA>;
using SpatialAccelerationABqA = SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointQ, CoordinateSystemA>;
using MotionAB = RigidMotion<CoordinateSystemB, CoordinateSystemA>;
using MotionBC = RigidMotion<CoordinateSystemC, CoordinateSystemB>;

// The point P, the velocities and the accelerations of the tests by hand: Q is 1 m above P along z.
const VelocityInA velocityOfP{1.0, 2.0, 3.0};
const AccelerationInA accelerationOfP{1.0, 2.0, 3.0};
const AngularVelocityInA angularVelocity{4.0, 1.0, 2.0};
const AngularAccelerationInA angularAcceleration{2.0, -1.0, 3.0};
const PositionInA offsetPQ{0.0, 0.0, 1.0};

const TimeQuantity step{1e-4};
const TimeQuantity time0{0.3};

// The motion of B in A, and of C in B, with angular and linear accelerations, so that every term of the formulas contributes.
const MotionAB motionAB{MotionAB::RotationT::fromEulerAnglesZyx(MotionAB::RotationT::EulerAnglesT{0.3, -0.4, 1.2}),
                        MotionAB::AxisT{1.0 / 3.0, 2.0 / 3.0, 2.0 / 3.0},
                        QuantityD<AngularVelocity>{0.7},
                        QuantityD<AngularAcceleration>{-0.4},
                        MotionAB::PositionT{0.5, -1.0, 2.0},
                        MotionAB::VelocityT{0.3, 0.2, -0.1},
                        MotionAB::AccelerationT{0.1, -0.2, 0.4}};
const MotionBC motionBC{MotionBC::RotationT::fromEulerAnglesZyx(MotionBC::RotationT::EulerAnglesT{-0.2, 0.5, 0.1}),
                        MotionBC::AxisT{0.0, 0.6, 0.8},
                        QuantityD<AngularVelocity>{-1.1},
                        QuantityD<AngularAcceleration>{0.6},
                        MotionBC::PositionT{1.0, 0.5, -0.5},
                        MotionBC::VelocityT{-0.4, 0.3, 0.2},
                        MotionBC::AccelerationT{0.2, 0.1, -0.3}};

// Whether the transport formulas take arguments of these types (see TakesTensorsOnly).
template <typename VelocityT_, typename AngularVelocityT_, typename OffsetT_>
concept HasVelocityOfBodyFixedPoint = requires(const VelocityT_& velocity, const AngularVelocityT_& angularVelocity,
                                               const OffsetT_& offset) { velocityOfBodyFixedPoint(velocity, angularVelocity, offset); };
template <typename AccelerationT_, typename AngularAccelerationT_, typename AngularVelocityT_, typename OffsetT_>
concept HasAccelerationOfBodyFixedPoint =
    requires(const AccelerationT_& acceleration, const AngularAccelerationT_& angularAcceleration, const AngularVelocityT_& angularVelocity,
             const OffsetT_& offset) { accelerationOfBodyFixedPoint(acceleration, angularAcceleration, angularVelocity, offset); };

}  // namespace

// The transport formulas take tensors only, so that the units, the coordinate systems and the absoluteness of their arguments are
// checked: they reject quantities, which have no coordinate system. Each check passes quantities for one argument only.
TEST(TestRigidBodyKinematics, TakesTensorsOnly) {  // NOLINT
  static_assert(HasVelocityOfBodyFixedPoint<VelocityInA, AngularVelocityInA, PositionInA>);
  static_assert(!HasVelocityOfBodyFixedPoint<VelocityInA::QuantitiesT, AngularVelocityInA, PositionInA>);
  static_assert(!HasVelocityOfBodyFixedPoint<VelocityInA, AngularVelocityInA::QuantitiesT, PositionInA>);
  static_assert(!HasVelocityOfBodyFixedPoint<VelocityInA, AngularVelocityInA, PositionInA::QuantitiesT>);
  static_assert(HasAccelerationOfBodyFixedPoint<AccelerationInA, AngularAccelerationInA, AngularVelocityInA, PositionInA>);
  static_assert(!HasAccelerationOfBodyFixedPoint<AccelerationInA::QuantitiesT, AngularAccelerationInA, AngularVelocityInA, PositionInA>);
  static_assert(!HasAccelerationOfBodyFixedPoint<AccelerationInA, AngularAccelerationInA::QuantitiesT, AngularVelocityInA, PositionInA>);
  static_assert(!HasAccelerationOfBodyFixedPoint<AccelerationInA, AngularAccelerationInA, AngularVelocityInA::QuantitiesT, PositionInA>);
  static_assert(!HasAccelerationOfBodyFixedPoint<AccelerationInA, AngularAccelerationInA, AngularVelocityInA, PositionInA::QuantitiesT>);
}

// v_Q = v_P + w x p_PQ, with w x p_PQ = (1, -4, 0) m/s.
TEST(TestRigidBodyKinematics, VelocityOfBodyFixedPoint) {  // NOLINT
  const VelocityInA expectedVelocity{2.0, -2.0, 3.0};
  EXPECT_EQ(velocityOfBodyFixedPoint(velocityOfP, angularVelocity, offsetPQ), expectedVelocity);

  const SpatialVelocityABpA spatialVelocityOfP{angularVelocity, velocityOfP};
  const SpatialVelocityABqA spatialVelocityOfQ = spatialVelocityOfP.shift<PointQ>(offsetPQ);
  EXPECT_EQ(spatialVelocityOfQ.getRotational(), angularVelocity);
  EXPECT_EQ(spatialVelocityOfQ.getTranslational(), expectedVelocity);
}

// a_Q = a_P + alpha x p_PQ + w x (w x p_PQ), with alpha x p_PQ = (-1, -2, 0) m/s^2 and w x (w x p_PQ) = (8, 2, -17) m/s^2.
TEST(TestRigidBodyKinematics, AccelerationOfBodyFixedPoint) {  // NOLINT
  const AccelerationInA expectedAcceleration{8.0, 2.0, -14.0};
  EXPECT_EQ(accelerationOfBodyFixedPoint(accelerationOfP, angularAcceleration, angularVelocity, offsetPQ), expectedAcceleration);

  const SpatialAccelerationABpA spatialAccelerationOfP{angularAcceleration, accelerationOfP};
  const SpatialAccelerationABqA spatialAccelerationOfQ =
      spatialAccelerationOfP.shift<PointQ>(offsetPQ, SpatialVelocityABpA{angularVelocity, velocityOfP});
  EXPECT_EQ(spatialAccelerationOfQ.getRotational(), angularAcceleration);
  EXPECT_EQ(spatialAccelerationOfQ.getTranslational(), expectedAcceleration);
}

// The velocity and the acceleration of a point Q fixed to the body B, which rotates with an angular acceleration, are the first
// and second derivatives of its position X_AB(t) p_BoQ_B: the tangential acceleration alpha x p and the centripetal acceleration
// w x (w x p) contribute.
TEST(TestRigidBodyKinematics, BodyFixedPointByCentralDifferences) {  // NOLINT
  const PointInB pointQInB{0.4, -0.3, 1.1};
  const auto positionOfQ = [&](const TimeQuantity& time) { return motionAB.poseAt(time0 + time).transform(pointQInB); };
  const VelocityInA numericalVelocity = centralDifference(step, positionOfQ);
  const AccelerationInA numericalAcceleration = secondCentralDifference(step, positionOfQ);

  // p_BoQ, expressed in A.
  const PositionInA offset = motionAB.rotationAt(time0).transform(makeRelative(pointQInB));
  const AngularVelocityInA angularVelocityOfB = motionAB.angularVelocityAt(time0);
  EXPECT_TRUE(velocityOfBodyFixedPoint(motionAB.velocityAt(time0), angularVelocityOfB, offset).isApprox(numericalVelocity, 1e-8));
  EXPECT_TRUE(accelerationOfBodyFixedPoint(motionAB.acceleration(), motionAB.angularAcceleration(), angularVelocityOfB, offset)
                  .isApprox(numericalAcceleration, 1e-6));
}

// The frame C moves relative to the body B: its origin moves and C rotates relative to B, so that the composition of the spatial
// accelerations has the Coriolis acceleration 2 w_AB x v_BCo and the angular acceleration w_AB x w_BC. They are checked against
// the derivatives of X_AC(t) = X_AB(t) X_BC(t). The spatial velocities and accelerations of C relative to B are known in B, and
// re-expressed in A.
TEST(TestRigidBodyKinematics, ComposeWithMovingFrameByCentralDifferences) {  // NOLINT
  using SpatialVelocityABA = SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, CoordinateSystemB, CoordinateSystemA>;
  using SpatialVelocityBCB = SpatialVelocityD<CoordinateSystemB, CoordinateSystemC, CoordinateSystemC, CoordinateSystemB>;
  using SpatialVelocityBCA = SpatialVelocityD<CoordinateSystemB, CoordinateSystemC, CoordinateSystemC, CoordinateSystemA>;
  using SpatialVelocityACA = SpatialVelocityD<CoordinateSystemA, CoordinateSystemC, CoordinateSystemC, CoordinateSystemA>;
  using SpatialAccelerationABA = SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, CoordinateSystemB, CoordinateSystemA>;
  using SpatialAccelerationBCB = SpatialAccelerationD<CoordinateSystemB, CoordinateSystemC, CoordinateSystemC, CoordinateSystemB>;
  using SpatialAccelerationBCA = SpatialAccelerationD<CoordinateSystemB, CoordinateSystemC, CoordinateSystemC, CoordinateSystemA>;
  using SpatialAccelerationACA = SpatialAccelerationD<CoordinateSystemA, CoordinateSystemC, CoordinateSystemC, CoordinateSystemA>;
  using RotationAC = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA>;

  // The derivatives of the motion of C in A.
  const auto originOfC = [&](const TimeQuantity& time) {
    return (motionAB.poseAt(time0 + time) * motionBC.poseAt(time0 + time)).transform(PointInC{0.0, 0.0, 0.0});
  };
  const auto rotationAC = [&](const TimeQuantity& time) -> RotationAC {
    return motionAB.rotationAt(time0 + time) * motionBC.rotationAt(time0 + time);
  };
  // w_AC = w_AB + w_BC, which the first check confirms, and whose derivative is the angular acceleration alpha_AC.
  const auto angularVelocityOfC = [&](const TimeQuantity& time) -> AngularVelocityInA {
    return motionAB.angularVelocityAt(time0 + time) + motionAB.rotationAt(time0 + time).transform(motionBC.angularVelocityAt(time0 + time));
  };
  const auto zero = step * 0.0;
  const AngularVelocityInA numericalAngularVelocity = rotationAC(zero).angularVelocityFromMatrixDerivative(
      centralDifference(step, [&](const TimeQuantity& time) { return rotationAC(time).getQuantities(); }));
  EXPECT_TRUE(angularVelocityOfC(zero).isApprox(numericalAngularVelocity, 1e-8));
  const SpatialVelocityACA numericalVelocity{numericalAngularVelocity, VelocityInA{centralDifference(step, originOfC)}};
  const SpatialAccelerationACA numericalAcceleration{AngularAccelerationInA{centralDifference(step, angularVelocityOfC)},
                                                     AccelerationInA{secondCentralDifference(step, originOfC)}};

  // The composition of the spatial vectors of B in A and of C in B.
  const SpatialVelocityABA velocityAB{motionAB.angularVelocityAt(time0), motionAB.velocityAt(time0)};
  const SpatialAccelerationABA accelerationAB{motionAB.angularAcceleration(), motionAB.acceleration()};
  const auto rotationAB = motionAB.rotationAt(time0);
  const SpatialVelocityBCA velocityBC =
      rotationAB.transform(SpatialVelocityBCB{motionBC.angularVelocityAt(time0), motionBC.velocityAt(time0)});
  const SpatialAccelerationBCA accelerationBC =
      rotationAB.transform(SpatialAccelerationBCB{motionBC.angularAcceleration(), motionBC.acceleration()});
  // p_BoCo, expressed in A.
  const PositionInA offset = rotationAB.transform(motionBC.positionAt(time0));
  const SpatialVelocityACA velocityAC = velocityAB.composeWithMovingFrameVelocity(offset, velocityBC);
  const SpatialAccelerationACA accelerationAC =
      accelerationAB.composeWithMovingFrameAcceleration(offset, velocityAB, velocityBC, accelerationBC);
  EXPECT_TRUE(velocityAC.isApprox(numericalVelocity, 1e-8));
  EXPECT_TRUE(accelerationAC.isApprox(numericalAcceleration, 1e-6));

  // The Coriolis acceleration matters: without it, the acceleration of Co is wrong.
  const AccelerationInA coriolis = velocityAB.getRotational().crossProduct(velocityBC.getTranslational()) * 2.0;
  EXPECT_FALSE((accelerationAC.getTranslational() - coriolis).isApprox(numericalAcceleration.getTranslational(), 1e-3));
}
