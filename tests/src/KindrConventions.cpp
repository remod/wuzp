#include <cmath>
#include <vector>

#include <gtest/gtest.h>
#include <Eigen/Core>
#include <kindr/Core>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "to_eigen.hpp"

using namespace wuzp;  // NOLINT

// The conventions of wuzp, compared with those of kindr (https://github.com/ANYbotics/kindr), which only builds if kindr is
// found, e.g. in the latest dev container (see "Comparison with kindr" in docs/conventions.md). kindr computes with Eigen, wuzp
// with the value matrix backend of the test.
//
// kindr's rotations are passive and their quaternions follow Hamilton's convention since kindr 1.0: the rotation C_IB maps the
// coordinates of a vector in the frame B to those in the frame I, I_r = C_IB B_r, and its parameters, e.g. its quaternion q_IB,
// are those of the matrix C_IB, the orientation of B relative to I. It is PassiveSpatialRotation<From = B, To = I> with the same
// parameters, 1:1: kindr's local angular velocity B_w_IB is expressed in From, its global angular velocity I_w_IB in To.

namespace {

using FrameI = CoordinateSystemA;
using FrameB = CoordinateSystemB;
using FrameC = CoordinateSystemC;
//! kindr's C_IB, which maps B to I.
using RotationIB = PassiveSpatialRotation3<ValueMatrix, double, FrameB, FrameI>;
using RotationBC = PassiveSpatialRotation3<ValueMatrix, double, FrameC, FrameB>;
using RotationIC = PassiveSpatialRotation3<ValueMatrix, double, FrameC, FrameI>;
using Active = ActiveSpatialRotation3<ValueMatrix, double, FrameI>;
using Quaternion = RotationIB::QuaternionT;
using EulerAngles = RotationIB::EulerAnglesT;
using EulerAnglesDerivative = RotationIB::EulerAnglesDerivativeT;
using RotationVectorInI = RotationVector3<ValueMatrix, double, FrameI>;
using RotationVectorInB = RotationVector3<ValueMatrix, double, FrameB>;
using AngularVelocityInI = AngularVelocityVector3<ValueMatrix, double, FrameI>;
using AngularVelocityInB = AngularVelocityVector3<ValueMatrix, double, FrameB>;
using PositionInI = PositionVector3<ValueMatrix, double, FrameI>;
using PositionInB = PositionVector3<ValueMatrix, double, FrameB>;

Quaternion toWuzp(const kindr::RotationQuaternionD& quaternion) {
  return Quaternion{quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z()};
}

Eigen::Vector4d toVector(const kindr::RotationQuaternionD& quaternion) {
  return Eigen::Vector4d{quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z()};
}

std::vector<kindr::RotationQuaternionD> sampleQuaternions() {
  return {kindr::RotationQuaternionD{}, kindr::RotationQuaternionD{Eigen::Quaterniond(0.4, -0.2, 0.5, 0.7).normalized()},
          kindr::RotationQuaternionD{Eigen::Quaterniond(-0.1, 0.9, 0.3, -0.2).normalized()},
          kindr::RotationQuaternionD{Eigen::Quaterniond(0.9, 0.1, -0.3, 0.2).normalized()}};
}

const Eigen::Vector3d sampleVector{0.3, -0.5, 0.9};

}  // namespace

// kindr's C_IB of the quaternion q_IB is the passive rotation from B to I of q_IB, and has the matrix of the active rotation of
// q_IB.
TEST(TestKindrConventions, PassiveRotationsAndQuaternions) {  // NOLINT
  for (const kindr::RotationQuaternionD& quaternion : sampleQuaternions()) {
    const Eigen::Matrix3d matrixIB = kindr::RotationMatrixD{quaternion}.matrix();
    const RotationIB rotation = RotationIB::fromQuaternion(toWuzp(quaternion));
    EXPECT_TRUE(toEigen(rotation.getQuantities()).isApprox(matrixIB, 1e-15));
    EXPECT_TRUE(toEigen(Active::fromQuaternion(toWuzp(quaternion)).getQuantities()).isApprox(matrixIB, 1e-15));
    // Both have a unique quaternion with a non-negative real part.
    EXPECT_TRUE(toEigen(rotation.toQuaternion()).isApprox(toVector(quaternion.getUnique()), 1e-14));
    // kindr's rotate() of C_IB maps B to I, like transform(), and inverseRotate() I to B, like transform() of the inverse.
    const PositionInB inB{sampleVector.x(), sampleVector.y(), sampleVector.z()};
    EXPECT_TRUE(toEigen(rotation.transform(inB).getQuantities()).isApprox(quaternion.rotate(sampleVector), 1e-15));
    const PositionInI inI{sampleVector.x(), sampleVector.y(), sampleVector.z()};
    EXPECT_TRUE(toEigen(rotation.inverse().transform(inI).getQuantities()).isApprox(quaternion.inverseRotate(sampleVector), 1e-15));
  }
}

// kindr composes C_IB * C_BC = C_IC, like wuzp's X_IB * X_BC = X_IC, with the same parameters.
TEST(TestKindrConventions, Composition) {  // NOLINT
  const std::vector<kindr::RotationQuaternionD> quaternions = sampleQuaternions();
  const kindr::RotationQuaternionD& quaternionIB = quaternions.at(1);
  const kindr::RotationQuaternionD& quaternionBC = quaternions.at(2);
  const kindr::RotationQuaternionD quaternionIC = quaternionIB * quaternionBC;
  const RotationIB rotationIB = RotationIB::fromQuaternion(toWuzp(quaternionIB));
  const RotationBC rotationBC = RotationBC::fromQuaternion(toWuzp(quaternionBC));
  EXPECT_TRUE((rotationIB * rotationBC).isApprox(RotationIC::fromQuaternion(toWuzp(quaternionIC)), 1e-14));
  EXPECT_TRUE(toEigen((rotationIB * rotationBC).getQuantities()).isApprox(kindr::RotationMatrixD{quaternionIC}.matrix(), 1e-14));
}

// Euler angles ZYX and XYZ are intrinsic in both. kindr converts a rotation into Euler angles with Eigen's eulerAngles(), whose
// first angle is in [0, pi]; its getUnique() has the ranges of wuzp, [-pi, pi), [-pi/2, pi/2) and [-pi, pi).
TEST(TestKindrConventions, EulerAngles) {  // NOLINT
  const kindr::EulerAnglesZyxD zyx{0.3, -0.4, 1.2};
  const kindr::EulerAnglesXyzD xyz{-2.0, 0.4, 2.5};
  const RotationIB fromZyx = RotationIB::fromEulerAnglesZyx(EulerAngles{zyx.yaw(), zyx.pitch(), zyx.roll()});
  const RotationIB fromXyz = RotationIB::fromEulerAnglesXyz(EulerAngles{xyz.roll(), xyz.pitch(), xyz.yaw()});
  EXPECT_TRUE(toEigen(fromZyx.getQuantities()).isApprox(kindr::RotationMatrixD{zyx}.matrix(), 1e-15));
  EXPECT_TRUE(toEigen(fromXyz.getQuantities()).isApprox(kindr::RotationMatrixD{xyz}.matrix(), 1e-15));
  for (const kindr::RotationQuaternionD& quaternion : sampleQuaternions()) {
    const RotationIB rotation = RotationIB::fromQuaternion(toWuzp(quaternion));
    EXPECT_TRUE(toEigen(rotation.toEulerAnglesZyx()).isApprox(kindr::EulerAnglesZyxD{quaternion}.getUnique().toImplementation(), 1e-14));
    EXPECT_TRUE(toEigen(rotation.toEulerAnglesXyz()).isApprox(kindr::EulerAnglesXyzD{quaternion}.getUnique().toImplementation(), 1e-14));
  }
  // At the boundary of [-pi, pi): an angle of pi is -pi in both.
  const Eigen::Matrix3d matrixAroundZ = Eigen::Vector3d{-1.0, -1.0, 1.0}.asDiagonal();
  const RotationIB aroundZ{-1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0};
  EXPECT_TRUE(toEigen(aroundZ.toEulerAnglesZyx())
                  .isApprox(kindr::EulerAnglesZyxD{kindr::RotationMatrixD{matrixAroundZ}}.getUnique().toImplementation(), 1e-15));
  EXPECT_TRUE(toEigen(aroundZ.toEulerAnglesXyz())
                  .isApprox(kindr::EulerAnglesXyzD{kindr::RotationMatrixD{matrixAroundZ}}.getUnique().toImplementation(), 1e-15));
}

// The axis and the rotation vector of C_IB are expressed in I, To, where their components are those in B.
TEST(TestKindrConventions, AngleAxisAndRotationVector) {  // NOLINT
  const kindr::AngleAxisD angleAxis{1.2, Eigen::Vector3d{0.2, -0.6, 0.4}.normalized()};
  const RotationIB fromAngleAxis = RotationIB::fromAngleAxis(
      RotationIB::AngleT{angleAxis.angle()}, RotationIB::AxisT{angleAxis.axis().x(), angleAxis.axis().y(), angleAxis.axis().z()});
  EXPECT_TRUE(toEigen(fromAngleAxis.getQuantities()).isApprox(kindr::RotationMatrixD{angleAxis}.matrix(), 1e-15));
  const kindr::RotationVectorD rotationVector{sampleVector};
  const RotationIB fromRotationVector =
      RotationIB::fromRotationVector(RotationVectorInI{sampleVector.x(), sampleVector.y(), sampleVector.z()});
  EXPECT_TRUE(toEigen(fromRotationVector.getQuantities()).isApprox(kindr::RotationMatrixD{rotationVector}.matrix(), 1e-15));
  // kindr's exponential and logarithmic maps are those of the rotation vector.
  kindr::RotationQuaternionD quaternion = sampleQuaternions().at(1);
  EXPECT_TRUE(toEigen(fromRotationVector.toQuaternion()).isApprox(toVector(quaternion.exponentialMap(sampleVector).getUnique()), 1e-15));
  EXPECT_TRUE(toEigen(RotationIB::fromQuaternion(toWuzp(quaternion)).toRotationVector().getQuantities())
                  .isApprox(quaternion.logarithmicMap(), 1e-14));
}

// kindr's boxPlus() and boxMinus() perturb on the left, in the frame I of C_IB, i.e. in To; the same increment in B, From, gives
// the same rotation.
TEST(TestKindrConventions, BoxPlusAndBoxMinus) {  // NOLINT
  const std::vector<kindr::RotationQuaternionD> quaternions = sampleQuaternions();
  const RotationVectorInI increment{sampleVector.x(), sampleVector.y(), sampleVector.z()};
  for (const kindr::RotationQuaternionD& quaternion : quaternions) {
    const RotationIB rotation = RotationIB::fromQuaternion(toWuzp(quaternion));
    const Eigen::Vector4d perturbed = toVector(quaternion.boxPlus(sampleVector).getUnique());
    EXPECT_TRUE(toEigen(rotation.boxPlus(increment).toQuaternion()).isApprox(perturbed, 1e-14));
    const RotationVectorInB incrementInB = rotation.inverse().transform(increment);
    EXPECT_TRUE(toEigen(rotation.boxPlus(incrementInB).toQuaternion()).isApprox(perturbed, 1e-14));
    const kindr::RotationQuaternionD& other = quaternions.at(1);
    const RotationIB otherRotation = RotationIB::fromQuaternion(toWuzp(other));
    EXPECT_LT((toEigen(rotation.boxMinus(otherRotation).getQuantities()) - quaternion.boxMinus(other)).norm(), 1e-14);
    EXPECT_NEAR(rotation.angularDistance(otherRotation).getValue(), quaternion.getDisparityAngle(other), 1e-14);
  }
  // The Jacobian of the exponential map is the same.
  EXPECT_TRUE(toEigen(RotationIB::exponentialMapJacobian(increment).getQuantities())
                  .isApprox(kindr::getJacobianOfExponentialMap(sampleVector), 1e-15));
  EXPECT_TRUE(
      toEigen(Active::exponentialMapJacobian(increment).getQuantities()).isApprox(kindr::getJacobianOfExponentialMap(sampleVector), 1e-15));
}

// kindr's C_IB.setFromVectors(B_v, I_v) is the passive rotation from B to I under which B_v has the direction of I_v, with the
// same quaternion and the same increments.
TEST(TestKindrConventions, FromTwoVectors) {  // NOLINT
  const Eigen::Vector3d inB{1.0, 2.0, 3.0};
  const Eigen::Vector3d inI{-1.0, 0.5, 2.0};
  kindr::RotationQuaternionD quaternionIB;
  quaternionIB.setFromVectors(inB, inI);
  const RotationIB rotation = RotationIB::fromTwoVectors(PositionInB{inB.x(), inB.y(), inB.z()}, PositionInI{inI.x(), inI.y(), inI.z()});
  EXPECT_TRUE(toEigen(rotation.getQuantities()).isApprox(kindr::RotationMatrixD{quaternionIB}.matrix(), 1e-14));
  EXPECT_TRUE(toEigen(rotation.toQuaternion()).isApprox(toVector(quaternionIB.getUnique()), 1e-14));
  const RotationVectorInI increment{sampleVector.x(), sampleVector.y(), sampleVector.z()};
  EXPECT_TRUE(
      toEigen(rotation.boxPlus(increment).toQuaternion()).isApprox(toVector(quaternionIB.boxPlus(sampleVector).getUnique()), 1e-14));
  kindr::RotationMatrixD matrixIB;
  matrixIB.setFromVectors(inB, inI);
  EXPECT_TRUE(toEigen(rotation.getQuantities()).isApprox(matrixIB.matrix(), 1e-14));
}

// kindr's local angular velocity B_w_IB is expressed in B, From of C_IB, and its global angular velocity I_w_IB in I, its To.
TEST(TestKindrConventions, Derivatives) {  // NOLINT
  const kindr::LocalAngularVelocityD local{sampleVector};
  const kindr::GlobalAngularVelocityD global{sampleVector};
  const AngularVelocityInB inB{sampleVector.x(), sampleVector.y(), sampleVector.z()};
  const AngularVelocityInI inI{sampleVector.x(), sampleVector.y(), sampleVector.z()};
  for (const kindr::RotationQuaternionD& quaternion : sampleQuaternions()) {
    const RotationIB rotation = RotationIB::fromQuaternion(toWuzp(quaternion));
    const Quaternion wuzpQuaternion = toWuzp(quaternion);
    // The derivative of the quaternion.
    const kindr::RotationQuaternionDiffD fromLocal{quaternion, local};
    const kindr::RotationQuaternionDiffD fromGlobal{quaternion, global};
    const Eigen::Vector4d fromLocalVector{fromLocal.w(), fromLocal.x(), fromLocal.y(), fromLocal.z()};
    const Eigen::Vector4d fromGlobalVector{fromGlobal.w(), fromGlobal.x(), fromGlobal.y(), fromGlobal.z()};
    EXPECT_TRUE(toEigen(RotationIB::quaternionDerivative(wuzpQuaternion, inB)).isApprox(fromLocalVector, 1e-15));
    EXPECT_TRUE(toEigen(RotationIB::quaternionDerivative(wuzpQuaternion, inI)).isApprox(fromGlobalVector, 1e-15));
    const RotationIB::QuaternionDerivativeT derivative = RotationIB::quaternionDerivative(wuzpQuaternion, inB);
    EXPECT_TRUE(toEigen(RotationIB::angularVelocityFromQuaternionDerivative<FrameB>(wuzpQuaternion, derivative).getQuantities())
                    .isApprox(kindr::LocalAngularVelocityD(quaternion, fromLocal).vector(), 1e-14));
    EXPECT_TRUE(toEigen(RotationIB::angularVelocityFromQuaternionDerivative(wuzpQuaternion, derivative).getQuantities())
                    .isApprox(kindr::GlobalAngularVelocityD(quaternion, fromLocal).vector(), 1e-14));
    // The derivative of the matrix, also of the active rotation with the global angular velocity.
    const kindr::RotationMatrixD matrix{quaternion};
    const Eigen::Matrix3d localMatrixDerivative = kindr::RotationMatrixDiffD{matrix, local}.toImplementation();
    const Eigen::Matrix3d globalMatrixDerivative = kindr::RotationMatrixDiffD{matrix, global}.toImplementation();
    EXPECT_TRUE(toEigen(rotation.matrixDerivative(inB)).isApprox(localMatrixDerivative, 1e-14));
    EXPECT_TRUE(toEigen(rotation.matrixDerivative(inI)).isApprox(globalMatrixDerivative, 1e-14));
    EXPECT_TRUE(toEigen(Active::fromQuaternion(wuzpQuaternion).matrixDerivative(inI)).isApprox(globalMatrixDerivative, 1e-14));
  }
  // The derivatives of the Euler angles.
  const kindr::EulerAnglesZyxD zyx{0.3, -0.4, 1.2};
  const EulerAngles zyxAngles{zyx.yaw(), zyx.pitch(), zyx.roll()};
  const kindr::EulerAnglesZyxDiffD zyxDerivative{zyx, local};
  EXPECT_TRUE(toEigen(RotationIB::eulerAnglesZyxDerivative(zyxAngles, inB)).isApprox(zyxDerivative.toImplementation(), 1e-14));
  EXPECT_TRUE(toEigen(RotationIB::angularVelocityFromEulerAnglesZyxDerivative<FrameB>(
                          zyxAngles, EulerAnglesDerivative{zyxDerivative.yaw(), zyxDerivative.pitch(), zyxDerivative.roll()})
                          .getQuantities())
                  .isApprox(kindr::LocalAngularVelocityD(zyx, zyxDerivative).vector(), 1e-14));
  const kindr::EulerAnglesXyzD xyz{-2.0, 0.4, 2.5};
  const EulerAngles xyzAngles{xyz.roll(), xyz.pitch(), xyz.yaw()};
  const kindr::EulerAnglesXyzDiffD xyzDerivative{xyz, local};
  EXPECT_TRUE(toEigen(RotationIB::eulerAnglesXyzDerivative(xyzAngles, inB)).isApprox(xyzDerivative.toImplementation(), 1e-14));
  const EulerAnglesDerivative xyzRates{xyzDerivative.roll(), xyzDerivative.pitch(), xyzDerivative.yaw()};
  EXPECT_TRUE(toEigen(RotationIB::angularVelocityFromEulerAnglesXyzDerivative(xyzAngles, xyzRates).getQuantities())
                  .isApprox(kindr::GlobalAngularVelocityD(xyz, xyzDerivative).vector(), 1e-14));
}

// The skew-symmetric matrix of a vector and the pseudo-inverse.
TEST(TestKindrConventions, LinearAlgebra) {  // NOLINT
  const PositionInI position{sampleVector.x(), sampleVector.y(), sampleVector.z()};
  const auto skew = skewSymmetricMatrix(position);
  EXPECT_TRUE(toEigen(skew.getQuantities()).isApprox(kindr::getSkewMatrixFromVector(sampleVector), 1e-15));
  EXPECT_TRUE(toEigen(vectorOfSkewSymmetricMatrix(skew).getQuantities())
                  .isApprox(kindr::getVectorFromSkewMatrix(toEigen(skew.getQuantities())), 1e-15));
  using Matrix = Quantities<ValueMatrix<double, 2, 3>, TypeMatrix<2, 3, Length>>;
  const Matrix matrix{1.0, 2.0, 3.0, -1.0, 0.5, 2.0};
  // kindr's pseudoInverse() takes dynamic matrices here: with fixed sizes, its singular value decomposition asserts in Eigen 5.
  Eigen::MatrixXd pseudoInverse;
  kindr::pseudoInverse(Eigen::MatrixXd{toEigen(matrix)}, pseudoInverse);
  EXPECT_TRUE(toEigen(matrix.pseudoInverse()).isApprox(pseudoInverse, 1e-14));
}
