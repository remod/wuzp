#include <string>
#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/SpatialAcceleration.hpp"
#include "wuzp/trafo/SpatialVelocity.hpp"

#include "coordinate_systems.hpp"
#include "spatial_vector_types.hpp"
#include "to_string.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

// Spatial accelerations (#76): their tags, sums, shifts, compositions and re-expressions, checked by hand with the formulas of
// Drake's SpatialAcceleration. RigidBodyKinematics.cpp checks them against central differences of rigid motions.

namespace {

using SpatialAccelerationABpA = SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA>;
using SpatialAccelerationABqA = SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointQ, CoordinateSystemA>;

// A_ABp and V_AB, about Q, whose angular velocity shifts A_ABp.
const SpatialAccelerationABpA accelerationAB{AngularAccelerationInA{1.0, 0.0, 2.0}, AccelerationInA{0.0, 1.0, 0.0}};
const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointQ, CoordinateSystemA> velocityAB{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                   VelocityInA{3.0, -1.0, 2.0}};
const PositionInA offsetPQ{1.0, 0.0, 0.0};

// Whether composeWithMovingFrameAcceleration() takes arguments of these types (see TakesTensorsOnly).
template <typename SpatialAccelerationT_, typename OffsetT_, typename... MotionT_>
concept ComposesWithMovingFrameAccelerationBy =
    requires(const SpatialAccelerationT_& spatialAcceleration, const OffsetT_& offset, const MotionT_&... motion) {
      spatialAcceleration.composeWithMovingFrameAcceleration(offset, motion...);
    };

}  // namespace

TEST(TestSpatialAcceleration, Types) {  // NOLINT
  static_assert(std::is_same_v<SpatialAccelerationABpA::RotationalT, AngularAccelerationInA>);
  static_assert(std::is_same_v<SpatialAccelerationABpA::TranslationalT, AccelerationInA>);
  static_assert(std::is_same_v<SpatialAccelerationABpA::OffsetT, PositionInA>);
  static_assert(std::is_same_v<GetCoordinateSystem<SpatialAccelerationABpA>::type, CoordinateSystemA>);
  static_assert(std::is_same_v<decltype(accelerationAB.shift<PointQ>(offsetPQ, velocityAB)), SpatialAccelerationABqA>);
  // A_ABp composed with V_BCq and A_BCq is A_ACq.
  static_assert(
      std::is_same_v<decltype(accelerationAB.composeWithMovingFrameAcceleration(
                         offsetPQ, velocityAB, SpatialVelocityD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemA>{},
                         SpatialAccelerationD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemA>{})),
                     SpatialAccelerationD<CoordinateSystemA, CoordinateSystemC, PointQ, CoordinateSystemA>>);
}

// The parts and the position vectors are tensors, like those of a spatial velocity: the constructor, shift() and
// composeWithMovingFrameAcceleration() reject quantities. Each check passes quantities for one argument only.
TEST(TestSpatialAcceleration, TakesTensorsOnly) {  // NOLINT
  using SpatialVelocityABqA = SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointQ, CoordinateSystemA>;
  using SpatialVelocityBCqA = SpatialVelocityD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemA>;
  using SpatialAccelerationBCqA = SpatialAccelerationD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemA>;
  static_assert(std::is_constructible_v<SpatialAccelerationABpA, AngularAccelerationInA, AccelerationInA>);
  static_assert(!std::is_constructible_v<SpatialAccelerationABpA, AngularAccelerationInA::QuantitiesT, AccelerationInA>);
  static_assert(!std::is_constructible_v<SpatialAccelerationABpA, AngularAccelerationInA, AccelerationInA::QuantitiesT>);
  static_assert(ShiftsToQBy<SpatialAccelerationABpA, PositionInA, SpatialVelocityABqA>);
  static_assert(!ShiftsToQBy<SpatialAccelerationABpA, PositionInA::QuantitiesT, SpatialVelocityABqA>);
  static_assert(ComposesWithMovingFrameAccelerationBy<SpatialAccelerationABpA, PositionInA, SpatialVelocityABqA, SpatialVelocityBCqA,
                                                      SpatialAccelerationBCqA>);
  static_assert(!ComposesWithMovingFrameAccelerationBy<SpatialAccelerationABpA, PositionInA::QuantitiesT, SpatialVelocityABqA,
                                                       SpatialVelocityBCqA, SpatialAccelerationBCqA>);
}

TEST(TestSpatialAcceleration, PartsAndSum) {  // NOLINT
  EXPECT_EQ(accelerationAB.getRotational(), (AngularAccelerationInA{1.0, 0.0, 2.0}));
  EXPECT_EQ(accelerationAB.getTranslational(), (AccelerationInA{0.0, 1.0, 0.0}));
  EXPECT_EQ(toString(accelerationAB), toString(accelerationAB.getRotational()) + "\n" + toString(accelerationAB.getTranslational()));
  const SpatialAccelerationABpA sum = accelerationAB + accelerationAB;
  EXPECT_EQ(sum, (SpatialAccelerationABpA{AngularAccelerationInA{2.0, 0.0, 4.0}, AccelerationInA{0.0, 2.0, 0.0}}));
  EXPECT_EQ(sum - accelerationAB, accelerationAB);
  EXPECT_TRUE(sum.isApprox(SpatialAccelerationABpA{AngularAccelerationInA{2.0, 0.0, 4.0 + 1e-14}, AccelerationInA{0.0, 2.0, 0.0}}, 1e-12));
  EXPECT_FALSE(sum.isApprox(accelerationAB, 1e-12));
}

// a_ABq = a_ABp + alpha_AB x p_PQ + w_AB x (w_AB x p_PQ), with alpha x p = (0, 2, 0) m/s^2 and w x (w x p) = (-1, 0, 0) m/s^2.
TEST(TestSpatialAcceleration, Shift) {  // NOLINT
  const SpatialAccelerationABqA shifted = accelerationAB.shift<PointQ>(offsetPQ, velocityAB);
  EXPECT_EQ(shifted.getRotational(), accelerationAB.getRotational());
  EXPECT_EQ(shifted.getTranslational(), (AccelerationInA{-1.0, 3.0, 0.0}));
  // There and back.
  EXPECT_EQ(shifted.shift<PointP>(offsetPQ * -1.0, velocityAB), accelerationAB);
}

// Drake's ComposeWithMovingFrameAcceleration(): alpha_AC = alpha_AB + alpha_BC + w_AB x w_BC, with w_AB x w_BC = (-1, 0, 0) 1/s^2,
// and a_ACq = a_ABp + alpha_AB x p + w_AB x (w_AB x p) + 2 w_AB x v_BCq + a_BCq, with the Coriolis acceleration 2 w_AB x v_BCq =
// (-4, 0, 0) m/s^2.
TEST(TestSpatialAcceleration, ComposeWithMovingFrameAcceleration) {  // NOLINT
  using SpatialVelocityBCqA = SpatialVelocityD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemA>;
  using SpatialAccelerationBCqA = SpatialAccelerationD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemA>;
  using SpatialAccelerationACqA = SpatialAccelerationD<CoordinateSystemA, CoordinateSystemC, PointQ, CoordinateSystemA>;
  const SpatialVelocityBCqA velocityBC{AngularVelocityInA{0.0, 1.0, 0.0}, VelocityInA{0.0, 2.0, 0.0}};
  const SpatialAccelerationBCqA accelerationBC{AngularAccelerationInA{1.0, 1.0, 0.0}, AccelerationInA{3.0, 0.0, 0.0}};
  EXPECT_EQ(accelerationAB.composeWithMovingFrameAcceleration(offsetPQ, velocityAB, velocityBC, accelerationBC),
            (SpatialAccelerationACqA{AngularAccelerationInA{1.0, 1.0, 2.0}, AccelerationInA{-2.0, 3.0, 0.0}}));

  // If C does not move relative to B, the composition is the shift.
  const SpatialAccelerationACqA composed = accelerationAB.composeWithMovingFrameAcceleration(
      offsetPQ, velocityAB, SpatialVelocityBCqA{AngularVelocityInA::zero(), VelocityInA::zero()},
      SpatialAccelerationBCqA{AngularAccelerationInA::zero(), AccelerationInA::zero()});
  const SpatialAccelerationABqA shifted = accelerationAB.shift<PointQ>(offsetPQ, velocityAB);
  EXPECT_EQ(composed.getRotational(), shifted.getRotational());
  EXPECT_EQ(composed.getTranslational(), shifted.getTranslational());
}

// A passive rotation re-expresses both parts, here in B, which is A rotated by pi/2 around z.
TEST(TestSpatialAcceleration, Reexpress) {  // NOLINT
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> rotation{0.0, 1.0, 0.0, -1.0, 0.0,
                                                                                                    0.0, 0.0, 0.0, 1.0};
  const SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemB> accelerationInB =
      rotation.transform(accelerationAB);
  EXPECT_EQ(accelerationInB.getRotational(), (AngularAccelerationInB{0.0, -1.0, 2.0}));
  EXPECT_EQ(accelerationInB.getTranslational(), (AccelerationInB{1.0, 0.0, 0.0}));
  EXPECT_EQ(rotation.inverse().transform(accelerationInB), accelerationAB);
}

// A passive rigid transformation X_AB re-expresses a spatial acceleration in A by its rotation alone, about the same point.
TEST(TestSpatialAcceleration, TransformByRigidTransformation) {  // NOLINT
  using RigidAB = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemB, CoordinateSystemA, double>;
  const RigidAB transformationAB{RigidAB::Rotation::fromEulerAnglesZyx(RigidAB::Rotation::EulerAnglesT{0.3, -0.4, 1.2}),
                                 RigidAB::Translation{0.5, -1.0, 2.0}};
  const SpatialAccelerationD<CoordinateSystemD, CoordinateSystemC, PointP, CoordinateSystemB> acceleration{
      AngularAccelerationInB{0.2, -0.9, 0.4}, AccelerationInB{1.1, 0.3, -0.6}};
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA> rotationAB{
      transformationAB.getRotation().getQuantities()};
  const SpatialAccelerationD<CoordinateSystemD, CoordinateSystemC, PointP, CoordinateSystemA> accelerationInA =
      transformationAB.transform(acceleration);
  EXPECT_EQ(accelerationInA, rotationAB.transform(acceleration));
}
