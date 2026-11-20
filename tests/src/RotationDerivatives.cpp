#include <cmath>
#include <numbers>
#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

// The time derivatives of rotations are checked against central differences of rotations whose parameters change at constant
// rates, which are accurate to the second order in the time step.

namespace {

using Rotation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
using PassiveRotation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
using EulerAngles = Rotation::EulerAnglesT;
using EulerAnglesDerivative = Rotation::EulerAnglesDerivativeT;
using AngularVelocityInA = Rotation::AngularVelocityT;
using AngularVelocityInB = AngularVelocityVector3<ValueMatrix, double, CoordinateSystemB>;
using Quaternion = Rotation::QuaternionT;
using QuaternionDerivative = Rotation::QuaternionDerivativeT;
using MatrixDerivative = Rotation::MatrixDerivativeT;
using RotationVector = Rotation::RotationVectorT;
using TimeQuantity = QuantityD<Time>;

const TimeQuantity step{1e-5};
const EulerAngles angles{0.3, -0.4, 1.2};
const EulerAnglesDerivative rates{0.5, -0.2, 0.7};

//! The central difference (f(h) - f(-h)) / 2h of a function of time, e.g. the quantities of a rotation.
template <typename FunctionT_>
auto centralDifference(const FunctionT_& function) {
  return (function(step) - function(step * -1.0)) / (step * 2.0);
}

}  // namespace

TEST(TestRotationDerivatives, Types) {  // NOLINT
  static_assert(std::is_same_v<MatrixDerivative, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, AngularVelocity>>>);
  static_assert(std::is_same_v<QuaternionDerivative, Quantities<ValueMatrix<double, 4, 1>, TypeMatrix<4, 1, AngularVelocity>>>);
  static_assert(std::is_same_v<decltype(Rotation{}.matrixDerivative(AngularVelocityInA{})), MatrixDerivative>);
  static_assert(std::is_same_v<decltype(Rotation::quaternionDerivative(Quaternion{}, AngularVelocityInA{})), QuaternionDerivative>);
  static_assert(std::is_same_v<decltype(PassiveRotation::angularVelocityFromQuaternionDerivative(Quaternion{}, QuaternionDerivative{})),
                               AngularVelocityInB>);
  static_assert(std::is_same_v<decltype(PassiveRotation::angularVelocityFromQuaternionDerivative<CoordinateSystemA>(
                                   Quaternion{}, QuaternionDerivative{})),
                               AngularVelocityInA>);
}

// dR/dt = [w]x R for a rotation that turns at the constant angular velocity w: R(t) = Exp(w t) R(0).
TEST(TestRotationDerivatives, MatrixDerivative) {  // NOLINT
  const AngularVelocityInA angularVelocity{0.3, -0.5, 0.9};
  const Rotation initial = Rotation::fromEulerAnglesZyx(angles);
  const MatrixDerivative numerical = centralDifference(
      [&](const TimeQuantity& time) { return (Rotation::fromRotationVector(angularVelocity * time) * initial).getQuantities(); });
  EXPECT_TRUE(initial.matrixDerivative(angularVelocity).isApprox(numerical, 1e-9));
  EXPECT_TRUE(initial.angularVelocityFromMatrixDerivative(numerical).isApprox(angularVelocity, 1e-9));
  EXPECT_TRUE(initial.angularVelocityFromMatrixDerivative(initial.matrixDerivative(angularVelocity)).isApprox(angularVelocity, 1e-14));
}

// The angular velocity of Euler angles ZYX that change at constant rates.
TEST(TestRotationDerivatives, EulerAnglesZyxDerivative) {  // NOLINT
  const AngularVelocityInA angularVelocity = Rotation::angularVelocityFromEulerAnglesZyxDerivative(angles, rates);
  const MatrixDerivative numerical =
      centralDifference([&](const TimeQuantity& time) { return Rotation::fromEulerAnglesZyx(angles + rates * time).getQuantities(); });
  EXPECT_TRUE(Rotation::fromEulerAnglesZyx(angles).matrixDerivative(angularVelocity).isApprox(numerical, 1e-9));
  EXPECT_TRUE(Rotation::eulerAnglesZyxDerivative(angles, angularVelocity).isApprox(rates, 1e-14));
  // Yaw alone turns around z.
  EXPECT_TRUE(Rotation::angularVelocityFromEulerAnglesZyxDerivative(angles, EulerAnglesDerivative{2.0, 0.0, 0.0})
                  .isApprox(AngularVelocityInA{0.0, 0.0, 2.0}, 1e-15));
}

TEST(TestRotationDerivatives, EulerAnglesXyzDerivative) {  // NOLINT
  const AngularVelocityInA angularVelocity = Rotation::angularVelocityFromEulerAnglesXyzDerivative(angles, rates);
  const MatrixDerivative numerical =
      centralDifference([&](const TimeQuantity& time) { return Rotation::fromEulerAnglesXyz(angles + rates * time).getQuantities(); });
  EXPECT_TRUE(Rotation::fromEulerAnglesXyz(angles).matrixDerivative(angularVelocity).isApprox(numerical, 1e-9));
  EXPECT_TRUE(Rotation::eulerAnglesXyzDerivative(angles, angularVelocity).isApprox(rates, 1e-14));
  // The first angle alone turns around x.
  EXPECT_TRUE(Rotation::angularVelocityFromEulerAnglesXyzDerivative(angles, EulerAnglesDerivative{2.0, 0.0, 0.0})
                  .isApprox(AngularVelocityInA{2.0, 0.0, 0.0}, 1e-15));
}

// At the gimbal lock, the derivatives of the Euler angles are not defined.
TEST(TestRotationDerivatives, EulerAnglesDerivativeAtGimbalLock) {  // NOLINT
  const AngularVelocityInA angularVelocity{1.0, 0.0, 0.0};
  const double rollRate =
      Rotation::eulerAnglesZyxDerivative(EulerAngles{0.0, std::numbers::pi / 2.0, 0.0}, angularVelocity).getValues().get(2, 0);
  EXPECT_TRUE(!std::isfinite(rollRate) || std::abs(rollRate) > 1e10);
}

// dq/dt = (0, w) q / 2.
TEST(TestRotationDerivatives, QuaternionDerivative) {  // NOLINT
  const AngularVelocityInA angularVelocity = Rotation::angularVelocityFromEulerAnglesZyxDerivative(angles, rates);
  const Quaternion quaternion = Rotation::fromEulerAnglesZyx(angles).toQuaternion();
  const QuaternionDerivative numerical =
      centralDifference([&](const TimeQuantity& time) { return Rotation::fromEulerAnglesZyx(angles + rates * time).toQuaternion(); });
  EXPECT_TRUE(Rotation::quaternionDerivative(quaternion, angularVelocity).isApprox(numerical, 1e-9));
  EXPECT_TRUE(Rotation::angularVelocityFromQuaternionDerivative(quaternion, numerical).isApprox(angularVelocity, 1e-9));
  // The derivative depends on the sign of the quaternion.
  EXPECT_TRUE(Rotation::quaternionDerivative(quaternion * -1.0, angularVelocity).isApprox(numerical * -1.0, 1e-9));
  EXPECT_TRUE(Rotation::angularVelocityFromQuaternionDerivative(quaternion * -1.0, numerical * -1.0).isApprox(angularVelocity, 1e-9));
}

// The Jacobian of the exponential map maps the derivative of the rotation vector to the angular velocity.
TEST(TestRotationDerivatives, ExponentialMapJacobian) {  // NOLINT
  const RotationVector rotationVector{1.0, 2.0, -0.5};
  const AngularVelocityInA rotationVectorDerivative{0.3, -0.1, 0.4};
  const MatrixDerivative numerical = centralDifference([&](const TimeQuantity& time) {
    return Rotation::fromRotationVector(rotationVector + rotationVectorDerivative * time).getQuantities();
  });
  const Rotation rotation = Rotation::fromRotationVector(rotationVector);
  EXPECT_TRUE(innerProduct(Rotation::exponentialMapJacobian(rotationVector), rotationVectorDerivative)
                  .isApprox(rotation.angularVelocityFromMatrixDerivative(numerical), 1e-9));
}

// The angular velocity of From relative to To of a passive rotation may be expressed in To, like kindr's global angular
// velocity, or in From, like kindr's local angular velocity: dR/dt = [w]x R = R [w']x.
TEST(TestRotationDerivatives, PassiveAngularVelocityInFromOrTo) {  // NOLINT
  const PassiveRotation rotation = PassiveRotation::fromEulerAnglesZyx(angles);
  const AngularVelocityInB inTo = PassiveRotation::angularVelocityFromEulerAnglesZyxDerivative(angles, rates);
  const AngularVelocityInA inFrom = PassiveRotation::angularVelocityFromEulerAnglesZyxDerivative<CoordinateSystemA>(angles, rates);
  EXPECT_TRUE(inTo.isApprox(rotation.transform(inFrom), 1e-14));
  // The active and the passive rotation with the same parameters have the same matrix, and the same angular velocity in To.
  EXPECT_TRUE(inTo.getQuantities().isApprox(Rotation::angularVelocityFromEulerAnglesZyxDerivative(angles, rates).getQuantities(), 1e-15));

  const MatrixDerivative numerical = centralDifference(
      [&](const TimeQuantity& time) { return PassiveRotation::fromEulerAnglesZyx(angles + rates * time).getQuantities(); });
  EXPECT_TRUE(rotation.matrixDerivative(inTo).isApprox(numerical, 1e-9));
  EXPECT_TRUE(rotation.matrixDerivative(inFrom).isApprox(numerical, 1e-9));
  EXPECT_TRUE(rotation.angularVelocityFromMatrixDerivative(numerical).isApprox(inTo, 1e-9));
  EXPECT_TRUE(rotation.angularVelocityFromMatrixDerivative<CoordinateSystemA>(numerical).isApprox(inFrom, 1e-9));

  EXPECT_TRUE(PassiveRotation::eulerAnglesZyxDerivative(angles, inTo).isApprox(rates, 1e-14));
  EXPECT_TRUE(PassiveRotation::eulerAnglesZyxDerivative(angles, inFrom).isApprox(rates, 1e-14));

  const Quaternion quaternion = rotation.toQuaternion();
  const QuaternionDerivative quaternionDerivative = centralDifference(
      [&](const TimeQuantity& time) { return PassiveRotation::fromEulerAnglesZyx(angles + rates * time).toQuaternion(); });
  EXPECT_TRUE(PassiveRotation::quaternionDerivative(quaternion, inTo).isApprox(quaternionDerivative, 1e-9));
  EXPECT_TRUE(PassiveRotation::quaternionDerivative(quaternion, inFrom).isApprox(quaternionDerivative, 1e-9));
  EXPECT_TRUE(PassiveRotation::angularVelocityFromQuaternionDerivative(quaternion, quaternionDerivative).isApprox(inTo, 1e-9));
  EXPECT_TRUE(
      PassiveRotation::angularVelocityFromQuaternionDerivative<CoordinateSystemA>(quaternion, quaternionDerivative).isApprox(inFrom, 1e-9));
}

TEST(TestRotationDerivatives, PassiveEulerAnglesXyz) {  // NOLINT
  const PassiveRotation rotation = PassiveRotation::fromEulerAnglesXyz(angles);
  const AngularVelocityInB inTo = PassiveRotation::angularVelocityFromEulerAnglesXyzDerivative(angles, rates);
  const AngularVelocityInA inFrom = PassiveRotation::angularVelocityFromEulerAnglesXyzDerivative<CoordinateSystemA>(angles, rates);
  EXPECT_TRUE(inTo.isApprox(rotation.transform(inFrom), 1e-14));
  const MatrixDerivative numerical = centralDifference(
      [&](const TimeQuantity& time) { return PassiveRotation::fromEulerAnglesXyz(angles + rates * time).getQuantities(); });
  EXPECT_TRUE(rotation.matrixDerivative(inFrom).isApprox(numerical, 1e-9));
  EXPECT_TRUE(PassiveRotation::eulerAnglesXyzDerivative(angles, inTo).isApprox(rates, 1e-14));
  EXPECT_TRUE(PassiveRotation::eulerAnglesXyzDerivative(angles, inFrom).isApprox(rates, 1e-14));
}
