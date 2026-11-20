#include <cmath>
#include <numbers>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

namespace {

constexpr double pi = std::numbers::pi;

using Rotation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
using PassiveRotation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
using RotationVector = Rotation::RotationVectorT;
using RotationVectorInB = RotationVector3<ValueMatrix, double, CoordinateSystemB>;
using RotationAngle = Rotation::AngleT;
using LinearMap = Rotation::TensorT;

std::vector<RotationVector> sampleRotationVectors() {
  return {RotationVector{0.0, 0.0, 0.0}, RotationVector{0.3, -0.2, 0.1}, RotationVector{1.0, 2.0, -0.5}, RotationVector{2.5, 0.3, 0.2},
          RotationVector{0.0, 0.0, pi - 1e-3}};
}

const RotationVector increment{0.01, -0.02, 0.03};

}  // namespace

// boxPlus rotates further, Exp(v) R, in the coordinate system of the active rotation, like kindr's boxPlus().
TEST(TestRotationManifold, BoxPlus) {  // NOLINT
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    const Rotation rotation = Rotation::fromRotationVector(rotationVector);
    static_assert(std::is_same_v<decltype(rotation.boxPlus(increment)), Rotation>);
    EXPECT_TRUE(rotation.boxPlus(increment).isApprox(Rotation::fromRotationVector(increment) * rotation, 1e-15));
    EXPECT_TRUE(Rotation{}.boxPlus(rotationVector).isApprox(rotation, 1e-15));
  }
}

// boxMinus is the inverse of boxPlus: other.boxPlus(rotation.boxMinus(other)) is the rotation.
TEST(TestRotationManifold, BoxMinus) {  // NOLINT
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    const Rotation rotation = Rotation::fromRotationVector(rotationVector);
    const Rotation other = Rotation::fromRotationVector(RotationVector{-0.4, 0.1, 0.7});
    static_assert(std::is_same_v<decltype(rotation.boxMinus(other)), RotationVector>);
    EXPECT_TRUE(other.boxPlus(rotation.boxMinus(other)).isApprox(rotation, 1e-14));
    EXPECT_TRUE(rotation.boxPlus(increment).boxMinus(rotation).isApprox(increment, 1e-12));
    if (rotationVector.norm().getValue() > 0.0) {
      EXPECT_TRUE(rotation.boxMinus(Rotation{}).isApprox(rotationVector, 1e-12));
    }
  }
  EXPECT_EQ(Rotation{}.boxMinus(Rotation{}), (RotationVector{0.0, 0.0, 0.0}));
}

// The angle between two rotations, like kindr's getDisparityAngle() and Eigen's angularDistance().
TEST(TestRotationManifold, AngularDistance) {  // NOLINT
  const Rotation::AxisT axis{0.0, 0.6, 0.8};
  const Rotation first = Rotation::fromAngleAxis(RotationAngle{0.1}, axis);
  const Rotation second = Rotation::fromAngleAxis(RotationAngle{0.4}, axis);
  static_assert(std::is_same_v<decltype(first.angularDistance(second)), RotationAngle>);
  EXPECT_TRUE(first.angularDistance(second).isApprox(RotationAngle{0.3}, 1e-14));
  EXPECT_TRUE(second.angularDistance(first).isApprox(RotationAngle{0.3}, 1e-14));
  EXPECT_NEAR(first.angularDistance(first).getValue(), 0.0, 1e-15);
  // The angle is at most pi.
  const Rotation opposite = Rotation::fromAngleAxis(RotationAngle{-2.0}, axis);
  EXPECT_TRUE(Rotation::fromAngleAxis(RotationAngle{2.0}, axis).angularDistance(opposite).isApprox(RotationAngle{2.0 * pi - 4.0}, 1e-14));
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    const Rotation rotation = Rotation::fromRotationVector(rotationVector);
    EXPECT_NEAR(rotation.angularDistance(second).getValue(), rotation.boxMinus(second).norm().getValue(), 1e-14);
  }
}

// The Jacobian of the exponential map: Exp(v + dv) = Exp(J dv) Exp(v) to first order, the left Jacobian of SO(3).
TEST(TestRotationManifold, ExponentialMapJacobian) {  // NOLINT
  static_assert(std::is_same_v<decltype(Rotation::exponentialMapJacobian(increment)), LinearMap>);
  EXPECT_EQ(Rotation::exponentialMapJacobian(RotationVector{0.0, 0.0, 0.0}), (LinearMap{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}));
  const double step = 1e-6;
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    const LinearMap jacobian = Rotation::exponentialMapJacobian(rotationVector);
    // J v = v, since Exp(v + t v) = Exp(t v) Exp(v).
    if (rotationVector.norm().getValue() > 0.0) {
      EXPECT_TRUE(innerProduct(jacobian, rotationVector).isApprox(rotationVector, 1e-14));
    }
    // The central difference Exp(v + h e_i) (-) Exp(v - h e_i) = 2 h J e_i, to second order in h.
    for (const RotationVector& direction : {RotationVector{1.0, 0.0, 0.0}, RotationVector{0.0, 1.0, 0.0}, RotationVector{0.0, 0.0, 1.0}}) {
      const RotationVector difference = Rotation::fromRotationVector(rotationVector + direction * step)
                                            .boxMinus(Rotation::fromRotationVector(rotationVector - direction * step));
      EXPECT_TRUE((difference / (2.0 * step)).isApprox(innerProduct(jacobian, direction), 1e-8));
    }
  }
  // A tiny rotation vector is accurate: J = I + [v]x / 2 to first order.
  const RotationVector tiny{1e-9, -2e-9, 3e-9};
  EXPECT_TRUE(
      Rotation::exponentialMapJacobian(tiny).isApprox(LinearMap{1.0, -1.5e-9, -1e-9, 1.5e-9, 1.0, -0.5e-9, 1e-9, 0.5e-9, 1.0}, 1e-15));
}

// The rotation vector of boxPlus of a passive rotation may be expressed in From or in To: the same vector gives the same result.
TEST(TestRotationManifold, PassiveBoxPlusInFromOrTo) {  // NOLINT
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    const RotationVectorInB inB{rotationVector.getQuantities()};
    const PassiveRotation rotation = PassiveRotation::fromRotationVector(inB);
    // The same increment, expressed in From (A) and in To (B), gives the same rotation.
    const RotationVectorInB incrementInB = rotation.transform(increment);
    EXPECT_TRUE(rotation.boxPlus(increment).isApprox(rotation.boxPlus(incrementInB), 1e-14));
    // The matrix is rotated further in To, like kindr's boxPlus() of C_TF, like that of the active rotation with the same matrix.
    EXPECT_TRUE(
        rotation.boxPlus(incrementInB)
            .getQuantities()
            .isApprox(Rotation::fromRotationVector(rotationVector).boxPlus(RotationVector{incrementInB.getQuantities()}).getQuantities(),
                      1e-15));
    EXPECT_TRUE(PassiveRotation{}.boxPlus(inB).isApprox(rotation, 1e-15));
  }
}

TEST(TestRotationManifold, PassiveBoxMinus) {  // NOLINT
  const PassiveRotation other = PassiveRotation::fromRotationVector(RotationVectorInB{-0.4, 0.1, 0.7});
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    const PassiveRotation rotation = PassiveRotation::fromRotationVector(RotationVectorInB{rotationVector.getQuantities()});
    static_assert(std::is_same_v<decltype(rotation.boxMinus(other)), RotationVectorInB>);
    static_assert(std::is_same_v<decltype(rotation.boxMinus<CoordinateSystemA>(other)), RotationVector>);
    EXPECT_TRUE(other.boxPlus(rotation.boxMinus(other)).isApprox(rotation, 1e-14));
    EXPECT_TRUE(other.boxPlus(rotation.boxMinus<CoordinateSystemA>(other)).isApprox(rotation, 1e-14));
    EXPECT_TRUE(rotation.boxMinus(other).isApprox(rotation.transform(rotation.boxMinus<CoordinateSystemA>(other)), 1e-14));
    EXPECT_TRUE(rotation.boxMinus(other).getQuantities().isApprox(
        Rotation::fromRotationVector(rotationVector).boxMinus(Rotation::fromRotationVector(RotationVector{-0.4, 0.1, 0.7})).getQuantities(),
        1e-14));
    EXPECT_NEAR(rotation.angularDistance(other).getValue(), rotation.boxMinus(other).norm().getValue(), 1e-14);
  }
}

TEST(TestRotationManifold, PassiveExponentialMapJacobian) {  // NOLINT
  using LinearMapInB =
      Tensor<Indices<Up, Down>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Dimensionless>>, CoordinateSystemB, Relative>;
  const RotationVectorInB rotationVector{1.0, 2.0, -0.5};
  const LinearMapInB jacobian = PassiveRotation::exponentialMapJacobian(rotationVector);
  EXPECT_TRUE(jacobian.getQuantities().isApprox(
      Rotation::exponentialMapJacobian(RotationVector{rotationVector.getQuantities()}).getQuantities(), 1e-15));
}

// In From, the Jacobian of the exponential map of a passive rotation is the right Jacobian J(-v) = J(v)^T, for which J dv is the
// increment of boxPlus() in From: Exp(v + h e) (-) Exp(v - h e) = 2 h J e in From, to second order.
TEST(TestRotationManifold, PassiveExponentialMapJacobianInFrom) {  // NOLINT
  using LinearMapInA = LinearMap;
  const double step = 1e-6;
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    const LinearMapInA jacobian = PassiveRotation::exponentialMapJacobian(rotationVector);
    EXPECT_TRUE(jacobian.getQuantities().isApprox(
        PassiveRotation::exponentialMapJacobian(RotationVectorInB{rotationVector.getQuantities()}).getQuantities().transpose(), 1e-15));
    for (const RotationVector& direction : {RotationVector{1.0, 0.0, 0.0}, RotationVector{0.0, 1.0, 0.0}, RotationVector{0.0, 0.0, 1.0}}) {
      const RotationVector difference =
          PassiveRotation::fromRotationVector(rotationVector + direction * step)
              .boxMinus<CoordinateSystemA>(PassiveRotation::fromRotationVector(rotationVector - direction * step));
      EXPECT_TRUE((difference / (2.0 * step)).isApprox(innerProduct(jacobian, direction), 1e-8));
    }
  }
}
