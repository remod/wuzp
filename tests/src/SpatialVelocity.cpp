#include <string>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"
#include "wuzp/trafo/SpatialAcceleration.hpp"
#include "wuzp/trafo/SpatialForce.hpp"
#include "wuzp/trafo/SpatialVector.hpp"
#include "wuzp/trafo/SpatialVelocity.hpp"

#include "central_difference.hpp"
#include "coordinate_systems.hpp"
#include "rigid_motion.hpp"
#include "spatial_vector_types.hpp"
#include "to_string.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

// Spatial velocities (#76): their tags, sums, shifts, compositions, re-expressions and the adjoint map of rigid transformations,
// checked by hand and against the twists of Modern Robotics (Lynch and Park, Cambridge University Press, 2017, section 3.3.2).

namespace {

using SpatialVelocityABpA = SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA>;
// R_BA, from A to B, and X_AB = (R_AB, p_AoBo_A), from B to A, in the monogram notation of Drake.
using RotationBA = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
using RigidAB = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemB, CoordinateSystemA, double>;
using RigidBC = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemC, CoordinateSystemB, double>;
template <typename CoordinateSystemT_>
using AngularVelocityLinearMap =
    Tensor<Indices<Up, Down>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, AngularVelocity>>, CoordinateSystemT_, Relative>;

// Whether composeWithMovingFrameVelocity() takes arguments of these types (see TakesTensorsOnly).
template <typename SpatialVelocityT_, typename OffsetT_, typename VelocityT_>
concept ComposesWithMovingFrameVelocityBy =
    requires(const SpatialVelocityT_& spatialVelocity, const OffsetT_& offset, const VelocityT_& velocity) {
      spatialVelocity.composeWithMovingFrameVelocity(offset, velocity);
    };

const SpatialVelocityABpA velocityOfP{AngularVelocityInA{1.0, 2.0, 3.0}, VelocityInA{4.0, 5.0, 6.0}};

// B is A rotated by pi/2 around z: R_AB maps x of B to y of A.
const RigidAB rigidAB{RigidAB::Rotation{0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}, RigidAB::Translation{1.0, 2.0, 3.0}};

}  // namespace

TEST(TestSpatialVelocity, Types) {  // NOLINT
  using SpatialVelocityABqA = SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointQ, CoordinateSystemA>;
  static_assert(std::is_same_v<SpatialVelocityABpA::RotationalT, AngularVelocityInA>);
  static_assert(std::is_same_v<SpatialVelocityABpA::TranslationalT, VelocityInA>);
  static_assert(std::is_same_v<SpatialVelocityABpA::OffsetT, PositionInA>);
  static_assert(std::is_same_v<GetCoordinateSystem<SpatialVelocityABpA>::type, CoordinateSystemA>);
  static_assert(std::is_same_v<decltype(velocityOfP.shift<PointQ>(PositionInA{})), SpatialVelocityABqA>);
  // V_ABp composed with V_BCq is V_ACq.
  static_assert(std::is_same_v<decltype(velocityOfP.composeWithMovingFrameVelocity(
                                   PositionInA{}, SpatialVelocityD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemA>{})),
                               SpatialVelocityD<CoordinateSystemA, CoordinateSystemC, PointQ, CoordinateSystemA>>);
  // A rotation and a rigid transformation only change the coordinate system the spatial velocity is expressed in.
  static_assert(std::is_same_v<decltype(RotationBA{}.transform(velocityOfP)),
                               SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemB>>);
  static_assert(
      std::is_same_v<decltype(rigidAB.transform(SpatialVelocityD<CoordinateSystemD, CoordinateSystemC, PointP, CoordinateSystemB>{})),
                     SpatialVelocityD<CoordinateSystemD, CoordinateSystemC, PointP, CoordinateSystemA>>);
  // The adjoint map of X_AB maps a spatial velocity about the origin of B, expressed in B, to the origin of A, expressed in A.
  static_assert(std::is_same_v<
                decltype(adjoint(rigidAB, SpatialVelocityD<CoordinateSystemD, CoordinateSystemC, CoordinateSystemB, CoordinateSystemB>{})),
                SpatialVelocityD<CoordinateSystemD, CoordinateSystemC, CoordinateSystemA, CoordinateSystemA>>);
}

// The parts and the position vectors are tensors, so that their units, coordinate systems and absoluteness are checked: the
// constructor, shift() and composeWithMovingFrameVelocity() reject quantities, which have no coordinate system. Each check passes
// quantities for one argument only.
TEST(TestSpatialVelocity, TakesTensorsOnly) {  // NOLINT
  using SpatialVelocityBCqA = SpatialVelocityD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemA>;
  static_assert(std::is_constructible_v<SpatialVelocityABpA, AngularVelocityInA, VelocityInA>);
  static_assert(!std::is_constructible_v<SpatialVelocityABpA, AngularVelocityInA::QuantitiesT, VelocityInA>);
  static_assert(!std::is_constructible_v<SpatialVelocityABpA, AngularVelocityInA, VelocityInA::QuantitiesT>);
  static_assert(ShiftsToQBy<SpatialVelocityABpA, PositionInA>);
  static_assert(!ShiftsToQBy<SpatialVelocityABpA, PositionInA::QuantitiesT>);
  static_assert(ComposesWithMovingFrameVelocityBy<SpatialVelocityABpA, PositionInA, SpatialVelocityBCqA>);
  static_assert(!ComposesWithMovingFrameVelocityBy<SpatialVelocityABpA, PositionInA::QuantitiesT, SpatialVelocityBCqA>);
}

// AnySpatialVector tells the spatial vectors from their parts and from the transformations that re-express them, also for a class
// that derives from a spatial vector.
TEST(TestSpatialVelocity, AnySpatialVector) {  // NOLINT
  struct DerivedVelocity : SpatialVelocityABpA {};
  static_assert(AnySpatialVector<SpatialVelocityABpA>);
  static_assert(AnySpatialVector<SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA>>);
  static_assert(AnySpatialVector<SpatialForceD<CoordinateSystemB, PointP, CoordinateSystemA>>);
  static_assert(AnySpatialVector<DerivedVelocity>);
  static_assert(!AnySpatialVector<VelocityInA>);
  static_assert(!AnySpatialVector<RigidAB>);
  static_assert(!AnySpatialVector<double>);
}

// The base class of the spatial vectors has a protected non-virtual destructor, like Transformation: spatial vectors are never
// deleted through a pointer to it. Its copies and moves are defaulted, since the destructor suppresses the implicit moves: a
// spatial vector is still moved without copying its parts, which would allocate with e.g. Blitz++ and could throw.
TEST(TestSpatialVelocity, BaseHasProtectedDestructor) {  // NOLINT
  using BaseT = SpatialVector<SpatialVelocityABpA, AngularVelocityInA, VelocityInA>;
  static_assert(std::is_base_of_v<BaseT, SpatialVelocityABpA>);
  static_assert(!std::is_polymorphic_v<BaseT>);
  static_assert(!std::is_destructible_v<BaseT>);
  using SpatialAccelerationABpA = SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA>;
  using SpatialForceBpA = SpatialForceD<CoordinateSystemB, PointP, CoordinateSystemA>;
  static_assert(std::is_nothrow_move_constructible_v<SpatialVelocityABpA>);
  static_assert(std::is_nothrow_move_assignable_v<SpatialVelocityABpA>);
  static_assert(std::is_nothrow_move_constructible_v<SpatialAccelerationABpA>);
  static_assert(std::is_nothrow_move_assignable_v<SpatialAccelerationABpA>);
  static_assert(std::is_nothrow_move_constructible_v<SpatialForceBpA>);
  static_assert(std::is_nothrow_move_assignable_v<SpatialForceBpA>);
  static_assert(std::is_copy_constructible_v<SpatialVelocityABpA>);
  static_assert(std::is_copy_assignable_v<SpatialVelocityABpA>);
}

TEST(TestSpatialVelocity, Parts) {  // NOLINT
  EXPECT_EQ(velocityOfP.getRotational(), (AngularVelocityInA{1.0, 2.0, 3.0}));
  EXPECT_EQ(velocityOfP.getTranslational(), (VelocityInA{4.0, 5.0, 6.0}));
  // The parts of a temporary are moved out of it.
  EXPECT_EQ(SpatialVelocityABpA{velocityOfP}.getRotational(), velocityOfP.getRotational());
  SpatialVelocityABpA copy = velocityOfP;
  EXPECT_EQ(std::move(copy).getTranslational(), velocityOfP.getTranslational());
  EXPECT_EQ(toString(velocityOfP), toString(velocityOfP.getRotational()) + "\n" + toString(velocityOfP.getTranslational()));
}

// Spatial velocities with the same tags are added and subtracted part by part, and compared like tensors.
TEST(TestSpatialVelocity, SumAndDifference) {  // NOLINT
  const SpatialVelocityABpA other{AngularVelocityInA{-1.0, 0.5, 2.0}, VelocityInA{1.0, -2.0, 0.0}};
  const SpatialVelocityABpA sum = velocityOfP + other;
  EXPECT_EQ(sum, (SpatialVelocityABpA{AngularVelocityInA{0.0, 2.5, 5.0}, VelocityInA{5.0, 3.0, 6.0}}));
  EXPECT_EQ(sum - other, velocityOfP);
  EXPECT_NE(sum, velocityOfP);
  EXPECT_TRUE(sum.isApprox(SpatialVelocityABpA{AngularVelocityInA{0.0, 2.5, 5.0}, VelocityInA{5.0, 3.0, 6.0 + 1e-14}}, 1e-12));
  EXPECT_FALSE(sum.isApprox(other, 1e-12));
  // Both parts are compared: the same angular velocity with another velocity differs.
  const SpatialVelocityABpA otherVelocity{velocityOfP.getRotational(), VelocityInA{4.0, 5.0, 7.0}};
  EXPECT_NE(otherVelocity, velocityOfP);
  EXPECT_FALSE(otherVelocity.isApprox(velocityOfP, 1e-12));
}

// v_MBq = v_MBp + w_MB x p_BpBq: w x p = (0, 2, 0) m/s.
TEST(TestSpatialVelocity, Shift) {  // NOLINT
  const SpatialVelocityABpA velocity{AngularVelocityInA{0.0, 0.0, 2.0}, VelocityInA{1.0, 0.0, 0.0}};
  const PositionInA offset{1.0, 0.0, 0.0};
  const auto shifted = velocity.shift<PointQ>(offset);
  EXPECT_EQ(shifted.getRotational(), velocity.getRotational());
  EXPECT_EQ(shifted.getTranslational(), (VelocityInA{1.0, 2.0, 0.0}));
  // There and back.
  EXPECT_EQ(shifted.shift<PointP>(offset * -1.0), velocity);
  EXPECT_EQ(velocityOfP.shift<PointQ>(PositionInA{0.5, -1.0, 2.0}).shift<PointP>(PositionInA{-0.5, 1.0, -2.0}), velocityOfP);
}

// Drake's ComposeWithMovingFrameVelocity(): w_AC = w_AB + w_BC and v_ACq = v_ABp + w_AB x p_BpCq + v_BCq, with w_AB x p_BpCq =
// (-2, 0, 0) m/s.
TEST(TestSpatialVelocity, ComposeWithMovingFrameVelocity) {  // NOLINT
  using SpatialVelocityBCqA = SpatialVelocityD<CoordinateSystemB, CoordinateSystemC, PointQ, CoordinateSystemA>;
  using SpatialVelocityACqA = SpatialVelocityD<CoordinateSystemA, CoordinateSystemC, PointQ, CoordinateSystemA>;
  const SpatialVelocityABpA velocityAB{AngularVelocityInA{0.0, 0.0, 1.0}, VelocityInA{1.0, 0.0, 0.0}};
  const PositionInA offset{0.0, 2.0, 0.0};
  const SpatialVelocityBCqA velocityBC{AngularVelocityInA{1.0, 0.0, 0.0}, VelocityInA{0.0, 0.0, 3.0}};
  EXPECT_EQ(velocityAB.composeWithMovingFrameVelocity(offset, velocityBC),
            (SpatialVelocityACqA{AngularVelocityInA{1.0, 0.0, 1.0}, VelocityInA{-1.0, 0.0, 3.0}}));

  // If C does not move relative to B, the composition is the shift.
  const SpatialVelocityACqA composed =
      velocityAB.composeWithMovingFrameVelocity(offset, SpatialVelocityBCqA{AngularVelocityInA::zero(), VelocityInA::zero()});
  const auto shifted = velocityAB.shift<PointQ>(offset);
  EXPECT_EQ(composed.getRotational(), shifted.getRotational());
  EXPECT_EQ(composed.getTranslational(), shifted.getTranslational());
}

// A passive rotation re-expresses both parts, here in B, which is A rotated by pi/2 around z.
TEST(TestSpatialVelocity, Reexpress) {  // NOLINT
  const RotationBA rotation{0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemB> velocityInB = rotation.transform(velocityOfP);
  EXPECT_EQ(velocityInB.getRotational(), (AngularVelocityInB{2.0, -1.0, 3.0}));
  EXPECT_EQ(velocityInB.getTranslational(), (VelocityInB{5.0, -4.0, 6.0}));
  EXPECT_EQ(rotation.inverse().transform(velocityInB), velocityOfP);
}

// A passive rigid transformation X_AB re-expresses a spatial velocity in A by its rotation R_AB alone, like a vector, since the
// point is the tag P rather than the origin of B: the translation leaves it unchanged, also on its own.
TEST(TestSpatialVelocity, TransformByRigidTransformation) {  // NOLINT
  const RigidAB transformationAB{RigidAB::Rotation::fromEulerAnglesZyx(RigidAB::Rotation::EulerAnglesT{0.3, -0.4, 1.2}),
                                 RigidAB::Translation{0.5, -1.0, 2.0}};
  const SpatialVelocityD<CoordinateSystemD, CoordinateSystemC, PointP, CoordinateSystemB> velocity{AngularVelocityInB{0.2, -0.9, 0.4},
                                                                                                   VelocityInB{1.1, 0.3, -0.6}};
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA> rotationAB{
      transformationAB.getRotation().getQuantities()};
  const SpatialVelocityD<CoordinateSystemD, CoordinateSystemC, PointP, CoordinateSystemA> velocityInA =
      transformationAB.transform(velocity);
  EXPECT_EQ(velocityInA, rotationAB.transform(velocity));
  // A temporary, which transform() passes on as one, is re-expressed the same way.
  EXPECT_EQ(transformationAB.transform(SpatialVelocityD<CoordinateSystemD, CoordinateSystemC, PointP, CoordinateSystemB>{
                AngularVelocityInB{0.2, -0.9, 0.4}, VelocityInB{1.1, 0.3, -0.6}}),
            velocityInA);

  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC> translation{1.0, 2.0, 3.0};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemC> velocityInC = translation.transform(velocityOfP);
  EXPECT_EQ(velocityInC.getRotational(), castCoordinateSystem<CoordinateSystemC>(velocityOfP.getRotational()));
  EXPECT_EQ(velocityInC.getTranslational(), castCoordinateSystem<CoordinateSystemC>(velocityOfP.getTranslational()));
}

// [Ad_X] of Modern Robotics: w' = R w and v' = R v + p x (R w), with R w = (0, 0, 1) 1/s, R v = (0, 1, 0) m/s and p x (R w) =
// (2, -1, 0) m/s.
TEST(TestSpatialVelocity, AdjointByHand) {  // NOLINT
  const SpatialVelocityD<CoordinateSystemD, CoordinateSystemC, CoordinateSystemB, CoordinateSystemB> velocity{
      AngularVelocityInB{0.0, 0.0, 1.0}, VelocityInB{1.0, 0.0, 0.0}};
  EXPECT_EQ(adjoint(rigidAB, velocity), (SpatialVelocityD<CoordinateSystemD, CoordinateSystemC, CoordinateSystemA, CoordinateSystemA>{
                                            AngularVelocityInA{0.0, 0.0, 1.0}, VelocityInA{2.0, 0.0, 0.0}}));
}

// The body twist [V_b] = T^-1 dT/dt and the spatial twist [V_s] = dT/dt T^-1 of a motion T(t) = X_AB(t) of Modern Robotics are the
// spatial velocities V_ABbo_B and V_ABao_A of B in A, about the origins of B and A, and the adjoint map of X_AB maps the first to
// the second, V_s = [Ad_T] V_b (section 3.3.2). The derivatives of T are central differences.
TEST(TestSpatialVelocity, AdjointMapsBodyTwistToSpatialTwist) {  // NOLINT
  using Motion = RigidMotion<CoordinateSystemB, CoordinateSystemA>;
  const Motion motion{Motion::RotationT::fromEulerAnglesZyx(Motion::RotationT::EulerAnglesT{0.3, -0.4, 1.2}),
                      Motion::AxisT{0.0, 0.6, -0.8},
                      QuantityD<AngularVelocity>{0.9},
                      QuantityD<AngularAcceleration>{-0.5},
                      Motion::PositionT{0.5, -1.0, 2.0},
                      Motion::VelocityT{0.3, 0.2, -0.1},
                      Motion::AccelerationT{0.1, -0.2, 0.4}};
  const TimeQuantity step{1e-4};
  const TimeQuantity time0{0.3};
  const auto rotation = motion.rotationAt(time0);
  const auto& matrix = rotation.getQuantities();
  const auto position = motion.positionAt(time0);
  const auto matrixDerivative =
      centralDifference(step, [&](const TimeQuantity& time) { return motion.rotationAt(time0 + time).getQuantities(); });
  const VelocityInA positionDerivative = centralDifference(step, [&](const TimeQuantity& time) { return motion.positionAt(time0 + time); });

  // [V_b] = T^-1 dT/dt = [[R^T dR/dt, R^T dp/dt], [0, 0]].
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, CoordinateSystemB, CoordinateSystemB> bodyTwist{
      vectorOfSkewSymmetricMatrix(AngularVelocityLinearMap<CoordinateSystemB>{innerProduct(matrix.transpose(), matrixDerivative)}),
      rotation.inverse().transform(positionDerivative)};
  // [V_s] = dT/dt T^-1 = [[dR/dt R^T, dp/dt - dR/dt R^T p], [0, 0]].
  const AngularVelocityInA spatialAngularVelocity =
      vectorOfSkewSymmetricMatrix(AngularVelocityLinearMap<CoordinateSystemA>{innerProduct(matrixDerivative, matrix.transpose())});
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, CoordinateSystemA, CoordinateSystemA> spatialTwist{
      spatialAngularVelocity, positionDerivative - spatialAngularVelocity.crossProduct(position)};

  const auto pose = motion.poseAt(time0);
  EXPECT_TRUE(adjoint(pose, bodyTwist).isApprox(spatialTwist, 1e-8));
  // The adjoint map is the re-expression by X_AB, which keeps the point, followed by the shift to the origin of A.
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, CoordinateSystemB, CoordinateSystemA> bodyTwistInA =
      pose.transform(bodyTwist);
  EXPECT_TRUE(adjoint(pose, bodyTwist).isApprox(bodyTwistInA.shift<CoordinateSystemA>(-position), 1e-14));
  EXPECT_TRUE(spatialTwist.getRotational().isApprox(motion.angularVelocityAt(time0), 1e-8));
  // The spatial twist is about the point of B at the origin of A: shifted to the origin of B, its velocity is dp/dt.
  EXPECT_TRUE(spatialTwist.shift<CoordinateSystemB>(position).getTranslational().isApprox(positionDerivative, 1e-12));
  // The angular velocity of R_AB, that of B relative to A, is the rotational part of both twists, in A and in B.
  EXPECT_TRUE(rotation.angularVelocityFromMatrixDerivative(matrixDerivative).isApprox(spatialTwist.getRotational(), 1e-12));
  EXPECT_TRUE(rotation.angularVelocityFromMatrixDerivative<CoordinateSystemB>(matrixDerivative).isApprox(bodyTwist.getRotational(), 1e-12));
}

// The adjoint map is a group homomorphism: [Ad_X_AB X_BC] = [Ad_X_AB] [Ad_X_BC], and [Ad_X^-1] = [Ad_X]^-1.
TEST(TestSpatialVelocity, AdjointOfCompositionAndInverse) {  // NOLINT
  const RigidAB transformationAB{RigidAB::Rotation::fromEulerAnglesZyx(RigidAB::Rotation::EulerAnglesT{0.3, -0.4, 1.2}),
                                 RigidAB::Translation{0.5, -1.0, 2.0}};
  const RigidBC transformationBC{RigidBC::Rotation::fromEulerAnglesXyz(RigidBC::Rotation::EulerAnglesT{-0.7, 0.2, 0.9}),
                                 RigidBC::Translation{-0.3, 0.8, 0.1}};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemD, CoordinateSystemC, CoordinateSystemC> velocity{
      AngularVelocityVector3<ValueMatrix, double, CoordinateSystemC>{0.2, -0.9, 0.4},
      VelocityVector3<ValueMatrix, double, CoordinateSystemC>{1.1, 0.3, -0.6}};
  EXPECT_TRUE(adjoint(transformationAB * transformationBC, velocity)
                  .isApprox(adjoint(transformationAB, adjoint(transformationBC, velocity)), 1e-12));
  EXPECT_TRUE(adjoint(transformationBC.inverse(), adjoint(transformationBC, velocity)).isApprox(velocity, 1e-12));
}
