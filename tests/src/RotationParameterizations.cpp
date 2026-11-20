#include <cmath>
#include <limits>
#include <numbers>
#include <type_traits>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/internal/validity.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "throws.hpp"

using namespace wuzp;  // NOLINT

namespace {

constexpr double pi = std::numbers::pi;

using Rotation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
using PassiveRotation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
using Quaternion = Rotation::QuaternionT;
using RotationAngle = Rotation::AngleT;
using Axis = Rotation::AxisT;
using AngleAxis = Rotation::AngleAxisT;
using RotationVector = Rotation::RotationVectorT;
using EulerAngles = Rotation::EulerAnglesT;
using Position = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
using RotationVectorInB = RotationVector3<ValueMatrix, double, CoordinateSystemB>;
using AxisInB = DirectionVector3<ValueMatrix, double, CoordinateSystemB>;

//! The rotation by pi/2 around x, as a matrix.
Rotation aroundX() {
  return Rotation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};
}

//! The rotation by an angle around a coordinate axis.
Rotation around(const double angle, const Axis& axis) {
  return Rotation::fromAngleAxis(RotationAngle{angle}, axis);
}

const Axis xAxis{1.0, 0.0, 0.0};
const Axis yAxis{0.0, 1.0, 0.0};
const Axis zAxis{0.0, 0.0, 1.0};

//! Rotations of every kind, as rotation vectors: the identity, by pi/2 and by pi around coordinate axes, close to pi, and general
//! ones, also with every element of the quaternion the largest, which the conversion from the matrix distinguishes.
std::vector<RotationVector> sampleRotationVectors() {
  return {RotationVector{0.0, 0.0, 0.0},       RotationVector{pi / 2.0, 0.0, 0.0}, RotationVector{0.0, pi, 0.0},
          RotationVector{0.0, 0.0, pi - 1e-6}, RotationVector{0.3, -0.2, 0.1},     RotationVector{1.0, 2.0, -0.5},
          RotationVector{2.5, 0.3, 0.2},       RotationVector{0.2, -2.6, 0.4},     RotationVector{-0.1, 0.5, 2.8}};
}

std::vector<Rotation> sampleRotations() {
  std::vector<Rotation> rotations;
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    rotations.push_back(Rotation::fromRotationVector(rotationVector));
  }
  return rotations;
}

}  // namespace

TEST(TestRotationParameterizations, Types) {  // NOLINT
  static_assert(std::is_same_v<Quaternion, Quantities<ValueMatrix<double, 4, 1>, TypeMatrix<4, 1, Dimensionless>>>);
  static_assert(std::is_same_v<RotationAngle, QuantityD<Angle>>);
  static_assert(std::is_same_v<Axis, DirectionVector3<ValueMatrix, double, CoordinateSystemA>>);
  static_assert(std::is_same_v<RotationVector, RotationVector3<ValueMatrix, double, CoordinateSystemA>>);
  static_assert(std::is_same_v<EulerAngles, Quantities<ValueMatrix<double, 3, 1>, TypeMatrix<3, 1, Angle>>>);
  static_assert(std::is_same_v<Rotation::AngularVelocityT, AngularVelocityVector3<ValueMatrix, double, CoordinateSystemA>>);
  // The vectors of a passive rotation are expressed in To.
  static_assert(std::is_same_v<PassiveRotation::RotationVectorT, RotationVectorInB>);
  static_assert(std::is_same_v<PassiveRotation::AngularVelocityT, AngularVelocityVector3<ValueMatrix, double, CoordinateSystemB>>);
}

// Hamilton's convention: the quaternion (cos(angle / 2), sin(angle / 2) n) rotates a vector v into q v q^* around n, so that
// the quaternion of the rotation by pi/2 around z rotates x into y.
TEST(TestRotationParameterizations, QuaternionOfHamilton) {  // NOLINT
  const double halfSqrt2 = std::sqrt(0.5);
  EXPECT_TRUE(Rotation::fromQuaternion(Quaternion{halfSqrt2, halfSqrt2, 0.0, 0.0}).isApprox(aroundX(), 1e-15));
  EXPECT_TRUE(aroundX().toQuaternion().isApprox(Quaternion{halfSqrt2, halfSqrt2, 0.0, 0.0}, 1e-15));
  const Rotation aroundZ = Rotation::fromQuaternion(Quaternion{halfSqrt2, 0.0, 0.0, halfSqrt2});
  EXPECT_TRUE(aroundZ.transform(Position{1.0, 0.0, 0.0}).isApprox(Position{0.0, 1.0, 0.0}, 1e-15));
  // The product of quaternions is the composition of rotations: q_z q_x rotates around x first.
  const Rotation composed = aroundZ * aroundX();
  EXPECT_TRUE(composed.toQuaternion().isApprox(Quaternion{0.5, 0.5, 0.5, 0.5}, 1e-15));
}

TEST(TestRotationParameterizations, QuaternionRoundTrip) {  // NOLINT
  for (const Rotation& rotation : sampleRotations()) {
    const Quaternion quaternion = rotation.toQuaternion();
    EXPECT_TRUE(Rotation::fromQuaternion(quaternion).isApprox(rotation, 1e-14));
    // q and -q are the same rotation.
    EXPECT_TRUE(Rotation::fromQuaternion(quaternion * -1.0).isApprox(rotation, 1e-14));
    EXPECT_NEAR(quaternion.getValues().norm(), 1.0, 1e-15);
  }
}

// The quaternion of a rotation is unique: its first element that is not zero is positive, like kindr's getUnique().
TEST(TestRotationParameterizations, QuaternionIsUnique) {  // NOLINT
  for (const Rotation& rotation : sampleRotations()) {
    EXPECT_GE(rotation.toQuaternion().getValues().get(0, 0), 0.0);
  }
  // By pi around x, y and z, and around -x: the real part is zero, and the first element of the axis is positive.
  EXPECT_EQ(Rotation(1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0).toQuaternion(), (Quaternion{0.0, 1.0, 0.0, 0.0}));
  EXPECT_EQ(Rotation(-1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0).toQuaternion(), (Quaternion{0.0, 0.0, 1.0, 0.0}));
  EXPECT_EQ(Rotation(-1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0).toQuaternion(), (Quaternion{0.0, 0.0, 0.0, 1.0}));
  EXPECT_EQ(Rotation::fromQuaternion(Quaternion{0.0, -1.0, 0.0, 0.0}).toQuaternion(), (Quaternion{0.0, 1.0, 0.0, 0.0}));
  EXPECT_EQ(Rotation::fromQuaternion(Quaternion{0.0, 0.0, -1.0, 0.0}).toQuaternion(), (Quaternion{0.0, 0.0, 1.0, 0.0}));
  EXPECT_EQ(Rotation::fromQuaternion(Quaternion{0.0, 0.0, 0.0, -1.0}).toQuaternion(), (Quaternion{0.0, 0.0, 0.0, 1.0}));
  EXPECT_EQ(Rotation{}.toQuaternion(), (Quaternion{1.0, 0.0, 0.0, 0.0}));
}

// A quaternion that does not have unit length is not a rotation, which the constructor checks in debug builds.
TEST(TestRotationParameterizations, QuaternionWithoutUnitLength) {  // NOLINT
  const Quaternion quaternion{2.0, 0.0, 0.0, 0.0};
  EXPECT_EQ(throwsInvalidArgument([&] { return Rotation::fromQuaternion(quaternion); }), wuzp::internal::checksValidity);
  EXPECT_EQ(throwsInvalidArgument([&] { return PassiveRotation::fromQuaternion(quaternion); }), wuzp::internal::checksValidity);
  // NaN elements fail the check too.
  const Quaternion notANumber{std::numeric_limits<double>::quiet_NaN(), 0.0, 0.0, 0.0};
  EXPECT_EQ(throwsInvalidArgument([&] { return Rotation::fromQuaternion(notANumber); }), wuzp::internal::checksValidity);
}

TEST(TestRotationParameterizations, AngleAxis) {  // NOLINT
  EXPECT_TRUE(around(pi / 2.0, xAxis).isApprox(aroundX(), 1e-15));
  const auto [angle, axis] = aroundX().toAngleAxis();
  EXPECT_TRUE(angle.isApprox(RotationAngle{pi / 2.0}, 1e-15));
  EXPECT_TRUE(axis.isApprox(xAxis, 1e-15));
  // A right-handed rotation: by pi/2 around z, x becomes y.
  EXPECT_TRUE(around(pi / 2.0, zAxis).transform(Position{1.0, 0.0, 0.0}).isApprox(Position{0.0, 1.0, 0.0}, 1e-15));
  // The axis of the identity is arbitrary: the x axis.
  const AngleAxis identity = Rotation{}.toAngleAxis();
  EXPECT_EQ(identity.first, RotationAngle{0.0});
  EXPECT_EQ(identity.second, xAxis);
  // The angle is in [0, pi]: a rotation by -pi/2 around x is a rotation by pi/2 around -x.
  const AngleAxis negative = around(-pi / 2.0, xAxis).toAngleAxis();
  EXPECT_TRUE(negative.first.isApprox(RotationAngle{pi / 2.0}, 1e-15));
  EXPECT_TRUE(negative.second.isApprox(Axis{-1.0, 0.0, 0.0}, 1e-15));
  for (const Rotation& rotation : sampleRotations()) {
    const AngleAxis angleAxis = rotation.toAngleAxis();
    EXPECT_TRUE(Rotation::fromAngleAxis(angleAxis.first, angleAxis.second).isApprox(rotation, 1e-14));
    EXPECT_GE(angleAxis.first.getValue(), 0.0);
    EXPECT_LE(angleAxis.first.getValue(), pi);
  }
  // An axis that does not have unit length gives a matrix that is not a rotation.
  EXPECT_EQ(throwsInvalidArgument([] { return Rotation::fromAngleAxis(RotationAngle{1.0}, Axis{2.0, 0.0, 0.0}); }),
            wuzp::internal::checksValidity);
  // Unless the angle is a multiple of 2 pi: then it is the identity, for every axis.
  for (const double multiple : {0.0, 2.0 * pi}) {
    const auto fromAngleAxis = [multiple] { return Rotation::fromAngleAxis(RotationAngle{multiple}, Axis{2.0, 0.0, 0.0}); };
    EXPECT_FALSE(throwsInvalidArgument(fromAngleAxis));
    EXPECT_TRUE(fromAngleAxis().isApprox(Rotation{}, 1e-15));
  }
}

// The rotation vector is the exponential coordinates of a rotation: the angle times the axis.
TEST(TestRotationParameterizations, RotationVector) {  // NOLINT
  EXPECT_TRUE(Rotation::fromRotationVector(RotationVector{pi / 2.0, 0.0, 0.0}).isApprox(aroundX(), 1e-15));
  EXPECT_TRUE(aroundX().toRotationVector().isApprox(RotationVector{pi / 2.0, 0.0, 0.0}, 1e-15));
  EXPECT_EQ(Rotation::fromRotationVector(RotationVector{0.0, 0.0, 0.0}), Rotation{});
  EXPECT_EQ(Rotation{}.toRotationVector(), (RotationVector{0.0, 0.0, 0.0}));
  const RotationVector general{1.0, 2.0, -0.5};
  const auto norm = general.norm();
  EXPECT_TRUE(Rotation::fromRotationVector(general).isApprox(Rotation::fromAngleAxis(norm, Axis{general / norm}), 1e-14));
  // A tiny rotation vector is accurate.
  const RotationVector tiny{1e-12, -2e-12, 3e-12};
  EXPECT_TRUE(Rotation::fromRotationVector(tiny).toRotationVector().isApprox(tiny, 1e-12));
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    const RotationVector logarithm = Rotation::fromRotationVector(rotationVector).toRotationVector();
    EXPECT_LE(logarithm.norm().getValue(), pi);
    if (rotationVector.norm().getValue() > 0.0 && rotationVector.norm().getValue() < pi) {
      EXPECT_TRUE(logarithm.isApprox(rotationVector, 1e-12));
    }
  }
  EXPECT_TRUE(Rotation::fromRotationVector(RotationVector{0.0, pi, 0.0}).toRotationVector().isApprox(RotationVector{0.0, pi, 0.0}, 1e-15));
}

// Euler angles ZYX (yaw, pitch, roll) are intrinsic: around z, then the new y, then the newest x, R = R_z R_y R_x.
TEST(TestRotationParameterizations, EulerAnglesZyx) {  // NOLINT
  const EulerAngles angles{0.3, 0.2, 0.1};
  const Rotation intrinsic = around(0.3, zAxis) * around(0.2, yAxis) * around(0.1, xAxis);
  EXPECT_TRUE(Rotation::fromEulerAnglesZyx(angles).isApprox(intrinsic, 1e-15));
  EXPECT_TRUE(intrinsic.toEulerAnglesZyx().isApprox(angles, 1e-14));
  // The same rotation, extrinsic: around the fixed axes x, y and z, in this order.
  const Position position{1.0, 2.0, 3.0};
  const Position extrinsic = around(0.3, zAxis).transform(around(0.2, yAxis).transform(around(0.1, xAxis).transform(position)));
  EXPECT_TRUE(Rotation::fromEulerAnglesZyx(angles).transform(position).isApprox(extrinsic, 1e-15));
  // Yaw alone rotates around z.
  EXPECT_TRUE(Rotation::fromEulerAnglesZyx(EulerAngles{pi / 2.0, 0.0, 0.0}).isApprox(around(pi / 2.0, zAxis), 1e-15));
  // The angles are in [-pi, pi], [-pi/2, pi/2] and [-pi, pi].
  const EulerAngles large{-2.5, 1.2, 3.0};
  EXPECT_TRUE(Rotation::fromEulerAnglesZyx(large).toEulerAnglesZyx().isApprox(large, 1e-14));
  for (const Rotation& rotation : sampleRotations()) {
    const EulerAngles roundTrip = rotation.toEulerAnglesZyx();
    EXPECT_TRUE(Rotation::fromEulerAnglesZyx(roundTrip).isApprox(rotation, 1e-14));
    EXPECT_LE(std::abs(roundTrip.getValues().get(1, 0)), pi / 2.0);
  }
}

// At the gimbal lock, a pitch of +-pi/2, only the difference or the sum of yaw and roll is defined, and the rotation stays exact.
TEST(TestRotationParameterizations, EulerAnglesZyxAtGimbalLock) {  // NOLINT
  for (const double pitch : {pi / 2.0, -pi / 2.0, pi / 2.0 - 1e-9}) {
    const Rotation rotation = Rotation::fromEulerAnglesZyx(EulerAngles{0.5, pitch, 0.2});
    const EulerAngles angles = rotation.toEulerAnglesZyx();
    EXPECT_TRUE(Rotation::fromEulerAnglesZyx(angles).isApprox(rotation, 1e-14));
    EXPECT_NEAR(angles.getValues().get(1, 0), pitch, 1e-7);
  }
}

// Euler angles XYZ are intrinsic: around x, then the new y, then the newest z, R = R_x R_y R_z.
TEST(TestRotationParameterizations, EulerAnglesXyz) {  // NOLINT
  const EulerAngles angles{0.3, 0.2, 0.1};
  const Rotation intrinsic = around(0.3, xAxis) * around(0.2, yAxis) * around(0.1, zAxis);
  EXPECT_TRUE(Rotation::fromEulerAnglesXyz(angles).isApprox(intrinsic, 1e-15));
  EXPECT_TRUE(intrinsic.toEulerAnglesXyz().isApprox(angles, 1e-14));
  const EulerAngles large{2.5, -1.2, -3.0};
  EXPECT_TRUE(Rotation::fromEulerAnglesXyz(large).toEulerAnglesXyz().isApprox(large, 1e-14));
  for (const Rotation& rotation : sampleRotations()) {
    const EulerAngles roundTrip = rotation.toEulerAnglesXyz();
    EXPECT_TRUE(Rotation::fromEulerAnglesXyz(roundTrip).isApprox(rotation, 1e-14));
    EXPECT_LE(std::abs(roundTrip.getValues().get(1, 0)), pi / 2.0);
  }
  for (const double second : {pi / 2.0, -pi / 2.0}) {
    const Rotation rotation = Rotation::fromEulerAnglesXyz(EulerAngles{0.5, second, 0.2});
    EXPECT_TRUE(Rotation::fromEulerAnglesXyz(rotation.toEulerAnglesXyz()).isApprox(rotation, 1e-14));
  }
}

TEST(TestRotationParameterizations, FloatValues) {  // NOLINT
  using RotationF = ActiveSpatialRotation3<ValueMatrix, float, CoordinateSystemA>;
  const RotationF::EulerAnglesT angles{0.3F, 0.2F, 0.1F};
  const RotationF rotation = RotationF::fromEulerAnglesZyx(angles);
  EXPECT_TRUE(rotation.toEulerAnglesZyx().isApprox(angles, 1e-5F));
  EXPECT_TRUE(RotationF::fromQuaternion(rotation.toQuaternion()).isApprox(rotation, 1e-5F));
}

// The rotation with the smallest angle that rotates one direction into the other.
TEST(TestRotationParameterizations, FromTwoVectors) {  // NOLINT
  const Position from{1.0, 2.0, 3.0};
  // Only the directions matter, so the units may differ.
  const VelocityVector3<ValueMatrix, double, CoordinateSystemA> to{-1.0, 0.5, 2.0};
  const Rotation rotation = Rotation::fromTwoVectors(from, to);
  EXPECT_TRUE(Axis{rotation.transform(from) / from.norm()}.isApprox(Axis{to / to.norm()}, 1e-14));
  // Its axis is perpendicular to both, and its angle is the angle between them.
  const AngleAxis angleAxis = rotation.toAngleAxis();
  EXPECT_NEAR(angleAxis.second.dotProduct(Axis{from / from.norm()}).getValue(), 0.0, 1e-15);
  EXPECT_NEAR(angleAxis.second.dotProduct(Axis{to / to.norm()}).getValue(), 0.0, 1e-15);
  EXPECT_NEAR(angleAxis.first.getValue(), std::acos((from.dotProduct(to) / (from.norm() * to.norm())).getValue()), 1e-14);
  // Parallel vectors: the identity.
  EXPECT_TRUE(Rotation::fromTwoVectors(from, from * 2.0).isApprox(Rotation{}, 1e-15));
  // Opposite vectors: a rotation by pi around an axis perpendicular to them.
  // The vectors cover every coordinate axis that the axis of the rotation is perpendicular to, the one along which the vector has
  // its smallest component, also when x is smaller than y but not than z.
  for (const Position& vector : {from, Position{0.0, 1.0, 0.0}, Position{0.0, 0.0, -2.0}, Position{3.0, 0.0, 0.0}, Position{3.0, 2.0, 1.0},
                                 Position{2.0, 3.0, 1.0}}) {
    const Rotation opposite = Rotation::fromTwoVectors(vector, vector * -1.0);
    EXPECT_TRUE(opposite.transform(vector).isApprox(vector * -1.0, 1e-15));
    EXPECT_TRUE(opposite.toAngleAxis().first.isApprox(RotationAngle{pi}, 1e-15));
  }
  // A zero vector has no direction.
  EXPECT_EQ(throwsInvalidArgument([&] { return Rotation::fromTwoVectors(from, Position{0.0, 0.0, 0.0}); }), wuzp::internal::checksValidity);
}

// The parameters of a passive rotation from From to To describe its matrix, the orientation of From relative to To, in the
// convention of robotics, e.g. the quaternion q_AB of Drake's R_AB and of kindr's C_AB: with a yaw of pi/2, From is To rotated
// by pi/2 around z, so the x axis of From is the y axis of To.
TEST(TestRotationParameterizations, PassiveParametersDescribeFromRelativeToTo) {  // NOLINT
  const PassiveRotation rotation = PassiveRotation::fromEulerAnglesZyx(EulerAngles{pi / 2.0, 0.0, 0.0});
  const PositionVector3<ValueMatrix, double, CoordinateSystemB> yAxisOfB{0.0, 1.0, 0.0};
  EXPECT_TRUE(rotation.transform(Position{1.0, 0.0, 0.0}).isApprox(yAxisOfB, 1e-15));
}

// The active and the passive rotation with the same parameters have the same matrix, for every parameterization.
TEST(TestRotationParameterizations, ActiveAndPassiveHaveTheSameMatrix) {  // NOLINT
  const auto expectSameMatrix = [](const Rotation& active, const PassiveRotation& passive) {
    EXPECT_TRUE(passive.getQuantities().isApprox(active.getQuantities(), 1e-15));
  };
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    const Rotation active = Rotation::fromRotationVector(rotationVector);
    expectSameMatrix(active, PassiveRotation::fromRotationVector(RotationVectorInB{rotationVector.getQuantities()}));
    expectSameMatrix(active, PassiveRotation::fromQuaternion(active.toQuaternion()));
    const AngleAxis angleAxis = active.toAngleAxis();
    expectSameMatrix(active, PassiveRotation::fromAngleAxis(angleAxis.first, AxisInB{angleAxis.second.getQuantities()}));
    expectSameMatrix(active, PassiveRotation::fromEulerAnglesZyx(active.toEulerAnglesZyx()));
    expectSameMatrix(active, PassiveRotation::fromEulerAnglesXyz(active.toEulerAnglesXyz()));
  }
}

// An active rotation R and the change of coordinates to the coordinate system B that it moves A onto cancel each other: the
// passive rotation from A to B, whose matrix is R^T, the inverse of the passive rotation from B to A with the parameters of R.
TEST(TestRotationParameterizations, ActiveAndPassiveCancelEachOther) {  // NOLINT
  using RotationBA = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA>;
  const Position position{1.0, 2.0, 3.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemB> positionInB{1.0, 2.0, 3.0};
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    const Rotation active = Rotation::fromRotationVector(rotationVector);
    const PassiveRotation passive = RotationBA::fromQuaternion(active.toQuaternion()).inverse();
    EXPECT_TRUE(passive.transform(active.transform(position)).isApprox(positionInB, 1e-14));
    EXPECT_TRUE(passive.getQuantities().isApprox(active.getQuantities().transpose(), 1e-15));
  }
}

TEST(TestRotationParameterizations, PassiveRoundTrips) {  // NOLINT
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    const PassiveRotation rotation = PassiveRotation::fromRotationVector(RotationVectorInB{rotationVector.getQuantities()});
    const Rotation active = Rotation::fromRotationVector(rotationVector);
    EXPECT_TRUE(PassiveRotation::fromQuaternion(rotation.toQuaternion()).isApprox(rotation, 1e-14));
    EXPECT_TRUE(rotation.toQuaternion().isApprox(active.toQuaternion(), 1e-14));
    const PassiveRotation::AngleAxisT angleAxis = rotation.toAngleAxis();
    EXPECT_TRUE(PassiveRotation::fromAngleAxis(angleAxis.first, angleAxis.second).isApprox(rotation, 1e-14));
    EXPECT_TRUE(PassiveRotation::fromRotationVector(rotation.toRotationVector()).isApprox(rotation, 1e-14));
    EXPECT_TRUE(PassiveRotation::fromEulerAnglesZyx(rotation.toEulerAnglesZyx()).isApprox(rotation, 1e-14));
    EXPECT_TRUE(rotation.toEulerAnglesZyx().isApprox(active.toEulerAnglesZyx(), 1e-14));
    EXPECT_TRUE(PassiveRotation::fromEulerAnglesXyz(rotation.toEulerAnglesXyz()).isApprox(rotation, 1e-14));
  }
}

// A passive rotation from two vectors expresses the vector in From as one with the direction of the vector in To, e.g. the
// gravity measured in a body frame and the gravity in the world frame.
TEST(TestRotationParameterizations, PassiveFromTwoVectors) {  // NOLINT
  const AccelerationVector3<ValueMatrix, double, CoordinateSystemA> measured{0.5, -0.3, 9.7};
  const AccelerationVector3<ValueMatrix, double, CoordinateSystemB> gravity{0.0, 0.0, 9.81};
  const PassiveRotation rotation = PassiveRotation::fromTwoVectors(measured, gravity);
  using DirectionInB = DirectionVector3<ValueMatrix, double, CoordinateSystemB>;
  EXPECT_TRUE(DirectionInB{rotation.transform(measured) / measured.norm()}.isApprox(DirectionInB{0.0, 0.0, 1.0}, 1e-14));
  // Its matrix is that of the active rotation between the components.
  const AccelerationVector3<ValueMatrix, double, CoordinateSystemA> gravityComponents{0.0, 0.0, 9.81};
  EXPECT_TRUE(rotation.getQuantities().isApprox(Rotation::fromTwoVectors(measured, gravityComponents).getQuantities(), 1e-15));
}

// Nearly opposite vectors keep the angle and the axis of the smallest rotation between them, here pi - delta around -y, with
// double and float values, rather than a rotation by pi around another perpendicular axis.
TEST(TestRotationParameterizations, FromNearlyOppositeVectors) {  // NOLINT
  for (const double delta : {1e-6, 3e-3}) {
    const Position from{1.0, 0.0, 0.0};
    const Position to{-std::cos(delta), 0.0, std::sin(delta)};
    const Rotation rotation = Rotation::fromTwoVectors(from, to);
    EXPECT_TRUE(rotation.transform(from).isApprox(to, 1e-9));
    const AngleAxis angleAxis = rotation.toAngleAxis();
    EXPECT_NEAR(angleAxis.first.getValue(), pi - delta, 1e-9);
    EXPECT_TRUE(angleAxis.second.isApprox(Axis{0.0, -1.0, 0.0}, 1e-9));
  }
  using RotationF = ActiveSpatialRotation3<ValueMatrix, float, CoordinateSystemA>;
  using PositionF = PositionVector3<ValueMatrix, float, CoordinateSystemA>;
  const PositionF from{1.0F, 0.0F, 0.0F};
  const PositionF to{-std::cos(3e-3F), 0.0F, std::sin(3e-3F)};
  const RotationF::AngleAxisT angleAxis = RotationF::fromTwoVectors(from, to).toAngleAxis();
  EXPECT_NEAR(angleAxis.first.getValue(), static_cast<float>(pi) - 3e-3F, 1e-4F);
  EXPECT_TRUE(angleAxis.second.isApprox(RotationF::AxisT{0.0F, -1.0F, 0.0F}, 1e-4F));
}

// Subnormal rotation vectors are normalized by a division, which does not overflow.
TEST(TestRotationParameterizations, SubnormalRotationVector) {  // NOLINT
  const RotationVector subnormal{1e-310, 0.0, 0.0};
  const Rotation rotation = Rotation::fromRotationVector(subnormal);
  EXPECT_TRUE(rotation.isValid());
  EXPECT_TRUE(rotation.toRotationVector().isApprox(subnormal, 1e-12));
  EXPECT_TRUE(rotation.toAngleAxis().second.isApprox(Axis{1.0, 0.0, 0.0}, 1e-15));
  EXPECT_TRUE(Rotation::exponentialMapJacobian(subnormal).isApprox(Rotation::exponentialMapJacobian(RotationVector{0.0, 0.0, 0.0}), 1e-15));
}

// The rotation vector and the axis of a passive rotation may be expressed in To or in From, where their components are the same.
TEST(TestRotationParameterizations, PassiveRotationVectorAndAxisInFromOrTo) {  // NOLINT
  for (const RotationVector& rotationVector : sampleRotationVectors()) {
    const PassiveRotation inTo = PassiveRotation::fromRotationVector(RotationVectorInB{rotationVector.getQuantities()});
    EXPECT_TRUE(PassiveRotation::fromRotationVector(rotationVector).isApprox(inTo, 1e-15));
    const AngleAxis angleAxis = Rotation::fromRotationVector(rotationVector).toAngleAxis();
    EXPECT_TRUE(PassiveRotation::fromAngleAxis(angleAxis.first, angleAxis.second).isApprox(inTo, 1e-14));
    // The rotation leaves its axis unchanged.
    EXPECT_TRUE(inTo.transform(angleAxis.second).getQuantities().isApprox(angleAxis.second.getQuantities(), 1e-14));
  }
}

// The first and the last Euler angle are in [-pi, pi), like kindr's unique Euler angles: an angle of pi is -pi.
TEST(TestRotationParameterizations, EulerAnglesAreHalfOpen) {  // NOLINT
  const Rotation aroundZ{-1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0};
  EXPECT_EQ(aroundZ.toEulerAnglesZyx().getValues().get(0, 0), -pi);
  EXPECT_EQ(aroundZ.toEulerAnglesXyz().getValues().get(2, 0), -pi);
  // The elements -0.0 give atan2(+0, -1) = pi for the roll and for the first angle XYZ.
  const Rotation aroundX{1.0, 0.0, 0.0, 0.0, -1.0, -0.0, 0.0, 0.0, -1.0};
  EXPECT_EQ(aroundX.toEulerAnglesZyx().getValues().get(2, 0), -pi);
  EXPECT_EQ(aroundX.toEulerAnglesXyz().getValues().get(0, 0), -pi);
  EXPECT_TRUE(Rotation::fromEulerAnglesZyx(aroundZ.toEulerAnglesZyx()).isApprox(aroundZ, 1e-15));
  EXPECT_TRUE(Rotation::fromEulerAnglesXyz(aroundX.toEulerAnglesXyz()).isApprox(aroundX, 1e-15));
}

// The directions of very long and very short vectors are computed without an overflow or an underflow of their norm.
TEST(TestRotationParameterizations, FromTwoVectorsOfExtremeMagnitudes) {  // NOLINT
  using RotationF = ActiveSpatialRotation3<ValueMatrix, float, CoordinateSystemA>;
  using PositionF = PositionVector3<ValueMatrix, float, CoordinateSystemA>;
  const RotationF aroundZ = RotationF::fromAngleAxis(RotationF::AngleT{static_cast<float>(pi) / 2.0F}, RotationF::AxisT{0.0F, 0.0F, 1.0F});
  for (const float magnitude : {2e19F, 1e-25F}) {
    const RotationF rotation = RotationF::fromTwoVectors(PositionF{magnitude, 0.0F, 0.0F}, PositionF{0.0F, magnitude, 0.0F});
    EXPECT_TRUE(rotation.isApprox(aroundZ, 1e-6F));
  }
  for (const double magnitude : {1e200, 1e-200}) {
    const Rotation rotation = Rotation::fromTwoVectors(Position{magnitude, 0.0, 0.0}, Position{0.0, magnitude, 0.0});
    EXPECT_TRUE(rotation.isApprox(around(pi / 2.0, zAxis), 1e-15));
    const PassiveRotation passive = PassiveRotation::fromTwoVectors(
        Position{magnitude, 0.0, 0.0}, PositionVector3<ValueMatrix, double, CoordinateSystemB>{0.0, magnitude, 0.0});
    EXPECT_TRUE(passive.getQuantities().isApprox(rotation.getQuantities(), 1e-15));
  }
}

// A quaternion that has unit length up to the tolerance is normalized, so that its matrix passes the validity check of the
// matrix, which would accept a deviation four times smaller close to an angle of pi.
TEST(TestRotationParameterizations, QuaternionWithinTheToleranceIsNormalized) {  // NOLINT
  const double almostOne = std::sqrt(1.0 + 0.9e-9);
  const Rotation fromQuaternion = Rotation::fromQuaternion(Quaternion{0.0, almostOne, 0.0, 0.0});
  EXPECT_TRUE(fromQuaternion.isValid());
  EXPECT_FALSE(throwsInvalidArgument([&] { return Rotation{fromQuaternion.getQuantities()}; }));
  EXPECT_TRUE(Rotation::fromAngleAxis(RotationAngle{pi}, Axis{almostOne, 0.0, 0.0}).isValid());
  EXPECT_TRUE(PassiveRotation::fromQuaternion(Quaternion{0.0, almostOne, 0.0, 0.0}).isValid());
  EXPECT_TRUE(PassiveRotation::fromAngleAxis(RotationAngle{pi}, AxisInB{almostOne, 0.0, 0.0}).isValid());
}
