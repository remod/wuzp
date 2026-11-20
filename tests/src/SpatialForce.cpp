#include <string>
#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/SpatialForce.hpp"
#include "wuzp/trafo/SpatialVelocity.hpp"

#include "coordinate_systems.hpp"
#include "spatial_vector_types.hpp"
#include "to_string.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

// Spatial forces (#76): their tags, sums, shifts, re-expressions and the adjoint map of rigid transformations, checked by hand and
// by the power with a spatial velocity, which does not depend on the point and the coordinate system (Drake's SpatialForce::dot()
// and Modern Robotics, Lynch and Park, Cambridge University Press, 2017, section 3.4).

namespace {

using SpatialForceBpA = SpatialForceD<CoordinateSystemB, PointP, CoordinateSystemA>;
using SpatialForceBqA = SpatialForceD<CoordinateSystemB, PointQ, CoordinateSystemA>;
using RigidAB = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemB, CoordinateSystemA, double>;

const SpatialForceBpA forceAtP{TorqueInA{0.3, -1.2, 0.5}, ForceInA{2.0, 0.4, -0.7}};

//! The power tau . w + f . v of a spatial force F_Bp_E and a spatial velocity V_MBp_E of the same frame, about the same point and
//! expressed in the same coordinate system, like Drake's SpatialForce::dot().
template <typename ForceT_, typename VelocityT_>
auto power(const ForceT_& force, const VelocityT_& velocity) {
  static_assert(std::is_same_v<typename ForceT_::FrameT, typename VelocityT_::FrameT> &&
                std::is_same_v<typename ForceT_::PointT, typename VelocityT_::PointT> &&
                std::is_same_v<typename ForceT_::ExpressedInT, typename VelocityT_::ExpressedInT>);
  return force.getRotational().dotProduct(velocity.getRotational()) + force.getTranslational().dotProduct(velocity.getTranslational());
}

}  // namespace

TEST(TestSpatialForce, Types) {  // NOLINT
  static_assert(std::is_same_v<SpatialForceBpA::RotationalT, TorqueInA>);
  static_assert(std::is_same_v<SpatialForceBpA::TranslationalT, ForceInA>);
  static_assert(std::is_same_v<SpatialForceBpA::OffsetT, PositionInA>);
  static_assert(std::is_same_v<GetCoordinateSystem<SpatialForceBpA>::type, CoordinateSystemA>);
  static_assert(std::is_same_v<decltype(forceAtP.shift<PointQ>(PositionInA{})), SpatialForceBqA>);
  static_assert(std::is_same_v<decltype(adjoint(RigidAB{}, SpatialForceD<CoordinateSystemC, CoordinateSystemB, CoordinateSystemB>{})),
                               SpatialForceD<CoordinateSystemC, CoordinateSystemA, CoordinateSystemA>>);
}

// The parts and the position vectors are tensors, like those of a spatial velocity: the constructor and shift() reject quantities.
// Each check passes quantities for one argument only.
TEST(TestSpatialForce, TakesTensorsOnly) {  // NOLINT
  static_assert(std::is_constructible_v<SpatialForceBpA, TorqueInA, ForceInA>);
  static_assert(!std::is_constructible_v<SpatialForceBpA, TorqueInA::QuantitiesT, ForceInA>);
  static_assert(!std::is_constructible_v<SpatialForceBpA, TorqueInA, ForceInA::QuantitiesT>);
  static_assert(ShiftsToQBy<SpatialForceBpA, PositionInA>);
  static_assert(!ShiftsToQBy<SpatialForceBpA, PositionInA::QuantitiesT>);
}

TEST(TestSpatialForce, PartsAndSum) {  // NOLINT
  EXPECT_EQ(forceAtP.getRotational(), (TorqueInA{0.3, -1.2, 0.5}));
  EXPECT_EQ(forceAtP.getTranslational(), (ForceInA{2.0, 0.4, -0.7}));
  EXPECT_EQ(toString(forceAtP), toString(forceAtP.getRotational()) + "\n" + toString(forceAtP.getTranslational()));
  // The forces on the same body about the same point add up.
  const SpatialForceBpA other{TorqueInA{1.0, 0.0, 0.0}, ForceInA{0.0, -1.0, 0.0}};
  const SpatialForceBpA sum = forceAtP + other;
  EXPECT_TRUE(sum.isApprox(SpatialForceBpA{TorqueInA{1.3, -1.2, 0.5}, ForceInA{2.0, -0.6, -0.7}}, 1e-15));
  EXPECT_TRUE((sum - other).isApprox(forceAtP, 1e-15));
  EXPECT_NE(sum, forceAtP);
}

// The force f = (0, 0, 1) N acts at P, so its torque about P is zero, and about Q = P + (1, 0, 0) m it is
// tau_Q = tau_P - p_PQ x f = (0, 1, 0) N m.
TEST(TestSpatialForce, Shift) {  // NOLINT
  const SpatialForceBpA force{TorqueInA::zero(), ForceInA{0.0, 0.0, 1.0}};
  const PositionInA offset{1.0, 0.0, 0.0};
  const SpatialForceBqA shifted = force.shift<PointQ>(offset);
  EXPECT_EQ(shifted.getRotational(), (TorqueInA{0.0, 1.0, 0.0}));
  EXPECT_EQ(shifted.getTranslational(), force.getTranslational());
  // There and back.
  EXPECT_EQ(shifted.shift<PointP>(offset * -1.0), force);
}

// A passive rotation re-expresses both parts, here in B, which is A rotated by pi/2 around z.
TEST(TestSpatialForce, Reexpress) {  // NOLINT
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> rotation{0.0, 1.0, 0.0, -1.0, 0.0,
                                                                                                    0.0, 0.0, 0.0, 1.0};
  const SpatialForceD<CoordinateSystemB, PointP, CoordinateSystemB> forceInB = rotation.transform(forceAtP);
  EXPECT_EQ(forceInB.getRotational(), (TorqueInB{-1.2, -0.3, 0.5}));
  EXPECT_EQ(forceInB.getTranslational(), (ForceInB{0.4, -2.0, -0.7}));
  EXPECT_EQ(rotation.inverse().transform(forceInB), forceAtP);
}

// A passive rigid transformation X_AB re-expresses a spatial force in A by its rotation alone, about the same point: unlike the
// adjoint map, it does not add the moment of the force about another point.
TEST(TestSpatialForce, TransformByRigidTransformation) {  // NOLINT
  const RigidAB transformationAB{RigidAB::Rotation::fromEulerAnglesZyx(RigidAB::Rotation::EulerAnglesT{0.3, -0.4, 1.2}),
                                 RigidAB::Translation{0.5, -1.0, 2.0}};
  const SpatialForceD<CoordinateSystemC, PointP, CoordinateSystemB> force{TorqueInB{0.3, -1.2, 0.5}, ForceInB{2.0, 0.4, -0.7}};
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA> rotationAB{
      transformationAB.getRotation().getQuantities()};
  const SpatialForceD<CoordinateSystemC, PointP, CoordinateSystemA> forceInA = transformationAB.transform(force);
  EXPECT_EQ(forceInA, rotationAB.transform(force));
}

// The wrench of Modern Robotics: f' = R f and tau' = R tau + p x (R f), with R tau = (0, 0, 1) N m, R f = (0, 1, 0) N and
// p x (R f) = (-3, 0, 1) N m.
TEST(TestSpatialForce, AdjointByHand) {  // NOLINT
  // B is A rotated by pi/2 around z: R_AB maps x of B to y of A.
  const RigidAB rigidAB{RigidAB::Rotation{0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}, RigidAB::Translation{1.0, 2.0, 3.0}};
  const SpatialForceD<CoordinateSystemC, CoordinateSystemB, CoordinateSystemB> force{TorqueInB{0.0, 0.0, 1.0}, ForceInB{1.0, 0.0, 0.0}};
  EXPECT_EQ(adjoint(rigidAB, force),
            (SpatialForceD<CoordinateSystemC, CoordinateSystemA, CoordinateSystemA>{TorqueInA{-3.0, 0.0, 2.0}, ForceInA{0.0, 1.0, 0.0}}));
}

// The power of a spatial force and a spatial velocity of the same frame about the same point does not depend on the point, nor
// on the coordinate system: shift() and adjoint() preserve it.
TEST(TestSpatialForce, PowerIsInvariant) {  // NOLINT
  const SpatialVelocityD<CoordinateSystemD, CoordinateSystemB, PointP, CoordinateSystemA> velocityOfP{AngularVelocityInA{0.2, 0.9, -0.4},
                                                                                                      VelocityInA{1.1, -0.3, 0.6}};
  const PositionInA offset{0.5, -0.8, 1.3};
  EXPECT_TRUE(power(forceAtP, velocityOfP).isApprox(power(forceAtP.shift<PointQ>(offset), velocityOfP.shift<PointQ>(offset)), 1e-13));

  const RigidAB transformationAB{RigidAB::Rotation::fromEulerAnglesZyx(RigidAB::Rotation::EulerAnglesT{0.3, -0.4, 1.2}),
                                 RigidAB::Translation{0.5, -1.0, 2.0}};
  const SpatialForceD<CoordinateSystemC, CoordinateSystemB, CoordinateSystemB> force{TorqueInB{0.3, -1.2, 0.5}, ForceInB{2.0, 0.4, -0.7}};
  const SpatialVelocityD<CoordinateSystemD, CoordinateSystemC, CoordinateSystemB, CoordinateSystemB> velocity{
      AngularVelocityInB{0.2, 0.9, -0.4}, VelocityInB{1.1, -0.3, 0.6}};
  EXPECT_TRUE(power(force, velocity).isApprox(power(adjoint(transformationAB, force), adjoint(transformationAB, velocity)), 1e-13));
}
