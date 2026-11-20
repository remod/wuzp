#include <cmath>
#include <numbers>
#include <vector>

#include <gtest/gtest.h>
#include <Eigen/Geometry>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "to_eigen.hpp"

using namespace wuzp;  // NOLINT

// The conventions of the rotations, compared with those of Eigen's geometry module (Eigen::Quaternion, Eigen::AngleAxis and
// MatrixBase::eulerAngles()), which the tests can use with every value matrix backend: Eigen computes the reference, and wuzp
// computes with the backend of the test. Like Eigen's, the matrices of both the active and the passive rotations are those of their
// parameters (see "Parameterizations of rotations" in DESIGN.md).

namespace {

using Rotation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
using PassiveRotation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
using Quaternion = Rotation::QuaternionT;
using RotationAngle = Rotation::AngleT;
using Axis = Rotation::AxisT;
using EulerAngles = Rotation::EulerAnglesT;
using Position = PositionVector3<ValueMatrix, double, CoordinateSystemA>;

std::vector<Eigen::Quaterniond> sampleQuaternions() {
  return {Eigen::Quaterniond::Identity(), Eigen::Quaterniond(0.4, -0.2, 0.5, 0.7).normalized(),
          Eigen::Quaterniond(-0.1, 0.9, 0.3, -0.2).normalized(), Eigen::Quaterniond(0.0, 0.0, 1.0, 0.0),
          Eigen::Quaterniond(0.2, 0.1, -0.3, 0.9).normalized()};
}

}  // namespace

// The quaternions of Eigen and wuzp follow Hamilton's convention, with the real part first in their constructors. Eigen stores
// the elements in the order (x, y, z, w) of coeffs(), though.
TEST(TestEigenConventions, Quaternion) {  // NOLINT
  const Position position{1.0, 2.0, 3.0};
  for (const Eigen::Quaterniond& eigen : sampleQuaternions()) {
    const Rotation rotation = Rotation::fromQuaternion(Quaternion{eigen.w(), eigen.x(), eigen.y(), eigen.z()});
    EXPECT_TRUE(toEigen(rotation.getQuantities()).isApprox(eigen.toRotationMatrix(), 1e-15));
    // Eigen's quaternion rotates a vector like the active rotation.
    EXPECT_TRUE(toEigen(rotation.transform(position)).isApprox(eigen * toEigen(position), 1e-15));
    // The quaternion of the matrix is the same, up to its sign.
    const Eigen::Quaterniond ofMatrix{toEigen(rotation.getQuantities())};
    const Quaternion quaternion = rotation.toQuaternion();
    const Quaternion eigenQuaternion{ofMatrix.w(), ofMatrix.x(), ofMatrix.y(), ofMatrix.z()};
    EXPECT_TRUE(eigenQuaternion.isApprox(quaternion, 1e-14) || (eigenQuaternion * -1.0).isApprox(quaternion, 1e-14));
    // The passive rotation with the quaternion has the same matrix, like Drake's RotationMatrix of a quaternion.
    EXPECT_TRUE(toEigen(PassiveRotation::fromQuaternion(quaternion).getQuantities()).isApprox(eigen.toRotationMatrix(), 1e-15));
  }
}

TEST(TestEigenConventions, AngleAxis) {  // NOLINT
  const Eigen::Vector3d axis = Eigen::Vector3d{0.2, -0.6, 0.4}.normalized();
  for (const double angle : {0.0, 0.3, -1.2, 2.5, std::numbers::pi}) {
    const Eigen::AngleAxisd eigen{angle, axis};
    const Rotation rotation = Rotation::fromAngleAxis(RotationAngle{angle}, Axis{axis.x(), axis.y(), axis.z()});
    EXPECT_TRUE(toEigen(rotation.getQuantities()).isApprox(eigen.toRotationMatrix(), 1e-15));
    // Both angles are in [0, pi].
    const Eigen::AngleAxisd ofMatrix{toEigen(rotation.getQuantities())};
    const Rotation::AngleAxisT angleAxis = rotation.toAngleAxis();
    EXPECT_NEAR(angleAxis.first.getValue(), ofMatrix.angle(), 1e-14);
    // The axis of the identity is arbitrary, and the sign of the axis of a rotation by pi.
    if (angle != 0.0) {
      EXPECT_TRUE(toEigen(angleAxis.second).isApprox(ofMatrix.axis(), 1e-14) ||
                  (angle == std::numbers::pi && toEigen(angleAxis.second).isApprox(-ofMatrix.axis(), 1e-14)));
    }
  }
}

// Euler angles ZYX (yaw, pitch, roll) and XYZ are intrinsic in both, like eulerAngles(2, 1, 0) and eulerAngles(0, 1, 2).
TEST(TestEigenConventions, EulerAngles) {  // NOLINT
  const EulerAngles angles{0.3, -0.4, 1.2};
  const double first = 0.3;
  const double second = -0.4;
  const double third = 1.2;
  const Eigen::Matrix3d zyx = (Eigen::AngleAxisd{first, Eigen::Vector3d::UnitZ()} * Eigen::AngleAxisd{second, Eigen::Vector3d::UnitY()} *
                               Eigen::AngleAxisd{third, Eigen::Vector3d::UnitX()})
                                  .toRotationMatrix();
  const Eigen::Matrix3d xyz = (Eigen::AngleAxisd{first, Eigen::Vector3d::UnitX()} * Eigen::AngleAxisd{second, Eigen::Vector3d::UnitY()} *
                               Eigen::AngleAxisd{third, Eigen::Vector3d::UnitZ()})
                                  .toRotationMatrix();
  EXPECT_TRUE(toEigen(Rotation::fromEulerAnglesZyx(angles).getQuantities()).isApprox(zyx, 1e-15));
  EXPECT_TRUE(toEigen(Rotation::fromEulerAnglesXyz(angles).getQuantities()).isApprox(xyz, 1e-15));
  // Eigen 5 deprecates eulerAngles(), whose first angle is in [0, pi], for canonicalEulerAngles(), whose ranges are those of
  // wuzp: [-pi, pi], [-pi/2, pi/2] and [-pi, pi]. Eigen 3.4 has only eulerAngles(), whose angles are compared through their
  // rotation.
#if EIGEN_VERSION_AT_LEAST(3, 4, 90)
  EXPECT_TRUE(toEigen(Rotation::fromEulerAnglesZyx(angles).toEulerAnglesZyx()).isApprox(zyx.canonicalEulerAngles(2, 1, 0), 1e-14));
  EXPECT_TRUE(toEigen(Rotation::fromEulerAnglesXyz(angles).toEulerAnglesXyz()).isApprox(xyz.canonicalEulerAngles(0, 1, 2), 1e-14));
#else
  const Eigen::Vector3d eigenZyx = zyx.eulerAngles(2, 1, 0);
  EXPECT_TRUE(Rotation::fromEulerAnglesZyx(EulerAngles{eigenZyx.x(), eigenZyx.y(), eigenZyx.z()})
                  .isApprox(Rotation::fromEulerAnglesZyx(angles), 1e-14));
  const Eigen::Vector3d eigenXyz = xyz.eulerAngles(0, 1, 2);
  EXPECT_TRUE(Rotation::fromEulerAnglesXyz(EulerAngles{eigenXyz.x(), eigenXyz.y(), eigenXyz.z()})
                  .isApprox(Rotation::fromEulerAnglesXyz(angles), 1e-14));
#endif
}

// fromTwoVectors() is Eigen's Quaternion::FromTwoVectors(), and angularDistance() Eigen's Quaternion::angularDistance().
TEST(TestEigenConventions, FromTwoVectorsAndAngularDistance) {  // NOLINT
  const Position from{1.0, 2.0, 3.0};
  const Position to{-1.0, 0.5, 2.0};
  const Eigen::Quaterniond eigen = Eigen::Quaterniond::FromTwoVectors(toEigen(from), toEigen(to));
  EXPECT_TRUE(toEigen(Rotation::fromTwoVectors(from, to).getQuantities()).isApprox(eigen.toRotationMatrix(), 1e-14));
  const std::vector<Eigen::Quaterniond> quaternions = sampleQuaternions();
  for (const Eigen::Quaterniond& lhs : quaternions) {
    for (const Eigen::Quaterniond& rhs : quaternions) {
      const Rotation lhsRotation = Rotation::fromQuaternion(Quaternion{lhs.w(), lhs.x(), lhs.y(), lhs.z()});
      const Rotation rhsRotation = Rotation::fromQuaternion(Quaternion{rhs.w(), rhs.x(), rhs.y(), rhs.z()});
      EXPECT_NEAR(lhsRotation.angularDistance(rhsRotation).getValue(), lhs.angularDistance(rhs), 1e-14);
    }
  }
}
