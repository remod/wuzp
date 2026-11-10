#include <gtest/gtest.h>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Jacobian.hpp"
#include "wuzp/trafo/LorentzTransformation.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "to_string.hpp"

using namespace wuzp;  // NOLINT

TEST(TestJacobian, TransformVelocityVector) {  // NOLINT
  constexpr int FromDimension = 3;
  constexpr int ToDimension = 2;
  using Object =
      Vector<Quantities<ValueMatrix<double, FromDimension, 1>, TypeMatrix<FromDimension, 1, Velocity, Velocity, AngularVelocity>>,
             CoordinateSystemA>;
  using AngleOverLength = Division<Angle, Length>::type;
  using LengthOverAngle = Inverse<AngleOverLength>::type;
  using Transformation = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, FromDimension, ToDimension, AngleOverLength,
                                  AngleOverLength, Dimensionless, Dimensionless, Dimensionless, LengthOverAngle>;
  using TransformedObject =
      Vector<Quantities<ValueMatrix<double, ToDimension, 1>, TypeMatrix<ToDimension, 1, AngularVelocity, Velocity>>, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{14.0, 32.0};
  const Transformation transformation{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

namespace {

template <typename TransformationT_>
concept HasGetTensor = requires(const TransformationT_& transformation) { transformation.getTensor(); };

}  // namespace

// The matrix of a Jacobian maps one coordinate system into another, possibly of another dimension: it is not a tensor in one
// coordinate system, also if it is square, so there is no getTensor(), only getQuantities().
TEST(TestJacobian, MatrixIsNotATensor) {  // NOLINT
  using NonSquare = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
  using Square = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 2, 2, Dimensionless>;
  static_assert(!HasGetTensor<NonSquare>);
  static_assert(!HasGetTensor<Square>);
  const NonSquare jacobian{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  EXPECT_EQ(jacobian.getQuantities(),
            (Quantities<ValueMatrix<double, 2, 3>, TypeMatrix<2, 3, Dimensionless>>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}));
  const Square square{1.0, 2.0, 3.0, 4.0};
  EXPECT_EQ(square.getQuantities(), (Quantities<ValueMatrix<double, 2, 2>, TypeMatrix<2, 2, Dimensionless>>{1.0, 2.0, 3.0, 4.0}));
}

TEST(TestJacobian, StreamOperator) {  // NOLINT
  const Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless> jacobian{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  EXPECT_EQ(toString(jacobian), toString(jacobian.getQuantities()));
}

// The matrices of transformations are compared as quantities, also those of Jacobians between coordinate systems of different
// dimensions, which are not tensors. isApprox is const and takes a precision, like that of tensors (#68).
TEST(TestJacobian, IsApprox) {  // NOLINT
  using NonSquare = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
  const NonSquare jacobian{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  EXPECT_TRUE(jacobian.isApprox(NonSquare{1.0, 2.0, 3.0, 4.0, 5.0, 6.0 + 1e-14}));
  EXPECT_FALSE(jacobian.isApprox(NonSquare{1.0, 2.0, 3.0, 4.0, 5.0, 7.0}));
  EXPECT_TRUE(jacobian.isApprox(NonSquare{1.0, 2.0, 3.0, 4.0, 5.0, 6.001}, 1e-3));
  EXPECT_TRUE(jacobian == (NonSquare{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}));
  EXPECT_TRUE(jacobian != (NonSquare{1.0, 2.0, 3.0, 4.0, 5.0, 7.0}));
}

namespace {

// The Jacobian of the Cartesian coordinates (x, y) in B with respect to the polar coordinates (r, phi) in A, where
// x = r cos(phi) and y = r sin(phi), at r = 2 m and phi = pi/2: (cos(phi), -r sin(phi); sin(phi), r cos(phi)). Its units are
// [x_i] / [q_j], i.e. m / m and m / rad.
using LengthOverAngle = Division<Length, Angle>::type;
using JacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 2, 2, Dimensionless, LengthOverAngle, Dimensionless,
                            LengthOverAngle>;
using VelocityInA = Vector<Quantities<ValueMatrix<double, 2, 1>, TypeMatrix<2, 1, Velocity, AngularVelocity>>, CoordinateSystemA>;
using VelocityInB = VelocityVector2<ValueMatrix, double, CoordinateSystemB>;
template <typename CoordinateSystemT_>
using ForceCovector = HomogeneousCovector<2, ValueMatrix, double, Force, CoordinateSystemT_>;
// The generalized forces of the polar coordinates: a force along r and a torque about the origin, per radian.
using GeneralizedForceInA =
    Covector<Quantities<ValueMatrix<double, 1, 2>, TypeMatrix<1, 2, Force, Division<Torque, Angle>::type>>, CoordinateSystemA>;
const JacobianBA jacobianBA{0.0, -2.0, 1.0, 0.0};
const VelocityInA velocityInA{1.0, 0.5};

}  // namespace

// The pushforward of a velocity is v' = J v: the radial velocity of 1 m/s points along y, and the angular velocity of 0.5 rad/s
// moves the point at r = 2 m with 1 m/s along -x.
TEST(TestJacobian, TransformVelocityOfPolarCoordinates) {  // NOLINT
  EXPECT_EQ(jacobianBA.transform(velocityInA), (VelocityInB{-1.0, 1.0}));
}

// A batch of vectors, one per column, is pushed forward column by column: the velocity above, and an angular velocity of 2 rad/s
// alone, which moves the point at r = 2 m with 4 m/s along -x.
TEST(TestJacobian, TransformBatchOfVelocities) {  // NOLINT
  using VelocitiesInA =
      Vector<Quantities<ValueMatrix<double, 2, 2>, TypeMatrix<2, 2, Velocity, Velocity, AngularVelocity, AngularVelocity>>,
             CoordinateSystemA>;
  using VelocitiesInB = Vector<Quantities<ValueMatrix<double, 2, 2>, TypeMatrix<2, 2, Velocity>>, CoordinateSystemB>;
  const VelocitiesInA velocities{1.0, 0.0, 0.5, 2.0};
  const VelocitiesInB transformedVelocities = jacobianBA.transform(velocities);
  EXPECT_EQ(transformedVelocities, (VelocitiesInB{-1.0, -4.0, 1.0, 0.0}));
}

// The pushforward of a tensor with two upper indices is T' = J T J^T, e.g. of a covariance S: S_x = J S_q J^T, with heterogeneous
// units in A, a length and an angle. The standard deviation of 0.1 m along r becomes one along y, and that of 0.05 rad along phi
// one of 0.1 m along x at r = 2 m.
TEST(TestJacobian, TransformCovariance) {  // NOLINT
  using LengthAngle = Product<Length, Angle>::type;
  using CovarianceInA =
      Tensor<Indices<Up, Up>,
             Quantities<ValueMatrix<double, 2, 2>, TypeMatrix<2, 2, Area, LengthAngle, LengthAngle, Product<Angle, Angle>::type>>,
             CoordinateSystemA, Relative>;
  using CovarianceInB = Tensor<Indices<Up, Up>, Quantities<ValueMatrix<double, 2, 2>, TypeMatrix<2, 2, Area>>, CoordinateSystemB, Relative>;
  const CovarianceInA covariance{0.01, 0.0, 0.0, 0.0025};
  const CovarianceInB transformedCovariance = jacobianBA.transform(covariance);
  EXPECT_TRUE(transformedCovariance.isApprox(CovarianceInB{0.01, 0.0, 0.0, 0.01}, 1e-12));
}

// The pullback of a covector is w J, e.g. the generalized forces tau = J^T F of the polar coordinates for a force F: a force of
// 4 N along y is a force of 4 N along r, and one of 3 N along x a torque of -6 N m about the origin. It pairs with the vectors
// like the covector it pulls back, so the power is the same in both coordinate systems: tau v = F (J v).
TEST(TestJacobian, PullbackCovector) {  // NOLINT
  const ForceCovector<CoordinateSystemB> force{3.0, 4.0};
  const GeneralizedForceInA generalizedForce = jacobianBA.pullback(force);
  EXPECT_EQ(generalizedForce, (GeneralizedForceInA{4.0, -6.0}));
  EXPECT_EQ(innerProduct(generalizedForce, velocityInA).getQuantity(),
            innerProduct(force, jacobianBA.transform(velocityInA)).getQuantity());
}

// The pullback of a tensor with two lower indices is J^T M J, e.g. of the mass matrix m I of a point mass in Cartesian
// coordinates: the mass matrix diag(m, m r^2) of the polar coordinates, with the kinetic energy m (r'^2 + r^2 phi'^2) / 2. The
// kinetic energy is the same in both coordinate systems: v^T (J^T M J) v = (J v)^T M (J v).
TEST(TestJacobian, PullbackMassMatrix) {  // NOLINT
  using MassLength = Product<Mass, LengthOverAngle>::type;
  using MassLength2 = Product<MassLength, LengthOverAngle>::type;
  using MassMatrixInA =
      Tensor<Indices<Down, Down>, Quantities<ValueMatrix<double, 2, 2>, TypeMatrix<2, 2, Mass, MassLength, MassLength, MassLength2>>,
             CoordinateSystemA, Relative>;
  using MassMatrixInB =
      Tensor<Indices<Down, Down>, Quantities<ValueMatrix<double, 2, 2>, TypeMatrix<2, 2, Mass>>, CoordinateSystemB, Relative>;
  const MassMatrixInB massMatrix{2.0, 0.0, 0.0, 2.0};
  const MassMatrixInA pulledBack = jacobianBA.pullback(massMatrix);
  EXPECT_EQ(pulledBack, (MassMatrixInA{2.0, 0.0, 0.0, 8.0}));
  const VelocityInB velocityInB = jacobianBA.transform(velocityInA);
  EXPECT_EQ(innerProduct(velocityInA, innerProduct(pulledBack, velocityInA)).getQuantity(),
            innerProduct(velocityInB, innerProduct(massMatrix, velocityInB)).getQuantity());
}

// The composition of Jacobians is the chain rule, J_CA = J_CB J_BA, e.g. of polar coordinates (r, phi) in A, Cartesian coordinates
// (x, y) in B, and the coordinates (x + y, x - y) in C.
TEST(TestJacobian, ChainRule) {  // NOLINT
  using JacobianCB = Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 2, 2, Dimensionless>;
  using JacobianCA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC, 2, 2, Dimensionless, LengthOverAngle,
                              Dimensionless, LengthOverAngle>;
  using VelocityInC = VelocityVector2<ValueMatrix, double, CoordinateSystemC>;
  const JacobianCB jacobianCB{1.0, 1.0, 1.0, -1.0};
  const JacobianCA jacobianCA = jacobianCB * jacobianBA;
  EXPECT_EQ(jacobianCA, (JacobianCA{1.0, -2.0, -1.0, -2.0}));
  EXPECT_EQ(jacobianCA.transform(velocityInA), jacobianCB.transform(jacobianBA.transform(velocityInA)));
  EXPECT_EQ(jacobianCA.transform(velocityInA), (VelocityInC{0.0, -2.0}));
  // Pulling back by the composition pulls back by each Jacobian in the opposite order: w J_CA = (w J_CB) J_BA.
  const ForceCovector<CoordinateSystemC> force{1.0, 2.0};
  EXPECT_EQ(jacobianCA.pullback(force), jacobianBA.pullback(jacobianCB.pullback(force)));
  EXPECT_EQ(jacobianCA.pullback(force), (GeneralizedForceInA{-1.0, -6.0}));
}

// The chain rule also holds between coordinate systems of different dimensions, e.g. for the Jacobian J_BA of two task space
// coordinates in B with respect to three joint coordinates in A, followed by the Jacobian J_CB of their sum: J_CA = J_CB J_BA, from
// three coordinates to one.
TEST(TestJacobian, ChainRuleNonSquare) {  // NOLINT
  using TaskJacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
  using SumJacobianCB = Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 2, 1, Dimensionless>;
  using SumJacobianCA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC, 3, 1, Dimensionless>;
  const TaskJacobianBA taskJacobianBA{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  const SumJacobianCB sumJacobianCB{1.0, 1.0};
  const SumJacobianCA sumJacobianCA = sumJacobianCB * taskJacobianBA;
  EXPECT_EQ(sumJacobianCA, (SumJacobianCA{5.0, 7.0, 9.0}));
  const VelocityVector3<ValueMatrix, double, CoordinateSystemA> velocity{1.0, 0.0, -1.0};
  EXPECT_EQ(sumJacobianCA.transform(velocity), sumJacobianCB.transform(taskJacobianBA.transform(velocity)));
  EXPECT_EQ(sumJacobianCA.transform(velocity), (VelocityVector<1, ValueMatrix, double, CoordinateSystemC>{-4.0}));
  const HomogeneousCovector<1, ValueMatrix, double, Force, CoordinateSystemC> force{2.0};
  EXPECT_EQ(sumJacobianCA.pullback(force), taskJacobianBA.pullback(sumJacobianCB.pullback(force)));
  EXPECT_EQ(sumJacobianCA.pullback(force), (HomogeneousCovector<3, ValueMatrix, double, Force, CoordinateSystemA>{10.0, 14.0, 18.0}));
}

// A change of basis composes with a Jacobian between coordinate systems of different dimensions on either side, e.g. with the task
// space Jacobian J_BA above: the passive rotation R_CB by -pi/2 expresses it in C, R_CB J_BA = J_CA, and the passive rotation R_AD
// by pi/2 around x gives the Jacobian with respect to the joint coordinates of D, J_BA R_AD = J_BD.
TEST(TestJacobian, ComposeNonSquareWithRotations) {  // NOLINT
  using TaskJacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
  using TaskJacobianCA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC, 3, 2, Dimensionless>;
  using TaskJacobianBD = Jacobian<ValueMatrix, double, CoordinateSystemD, CoordinateSystemB, 3, 2, Dimensionless>;
  const TaskJacobianBA taskJacobianBA{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  const PassiveSpatialRotation2<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC> rotationCB{0.0, 1.0, -1.0, 0.0};
  const TaskJacobianCA taskJacobianCA = rotationCB * taskJacobianBA;
  EXPECT_EQ(taskJacobianCA, (TaskJacobianCA{4.0, 5.0, 6.0, -1.0, -2.0, -3.0}));
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA> rotationAD{1.0, 0.0, 0.0,  0.0, 0.0,
                                                                                                      1.0, 0.0, -1.0, 0.0};
  const TaskJacobianBD taskJacobianBD = taskJacobianBA * rotationAD;
  EXPECT_EQ(taskJacobianBD, (TaskJacobianBD{1.0, -3.0, 2.0, 4.0, -6.0, 5.0}));
  const VelocityVector3<ValueMatrix, double, CoordinateSystemD> velocity{1.0, 2.0, 3.0};
  EXPECT_EQ(taskJacobianBD.transform(velocity), taskJacobianBA.transform(rotationAD.transform(velocity)));
  EXPECT_EQ(taskJacobianBD.transform(velocity), (VelocityVector2<ValueMatrix, double, CoordinateSystemB>{1.0, 7.0}));
}

// A change of basis is the Jacobian of the identity map between two coordinate systems, so it composes with Jacobians into
// Jacobians by the chain rule, in both orders. E.g. the passive rotation R_CB by -pi/2 expresses J_BA in C, R_CB J_BA = J_CA.
TEST(TestJacobian, ComposeWithRotation) {  // NOLINT
  using RotationCB = PassiveSpatialRotation2<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC>;
  using JacobianCA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC, 2, 2, Dimensionless, LengthOverAngle,
                              Dimensionless, LengthOverAngle>;
  const RotationCB rotationCB{0.0, 1.0, -1.0, 0.0};
  const JacobianCA jacobianCA = rotationCB * jacobianBA;
  EXPECT_EQ(jacobianCA, (JacobianCA{1.0, 0.0, 0.0, 2.0}));
  EXPECT_EQ(jacobianCA.transform(velocityInA), rotationCB.transform(jacobianBA.transform(velocityInA)));
  // A rotation of the coordinates of A, R_AD, on the right: J_BD = J_BA R_AD, e.g. of Cartesian coordinates rotated by -pi/2.
  using RotationBD = PassiveSpatialRotation2<ValueMatrix, double, CoordinateSystemD, CoordinateSystemB>;
  using JacobianCD = Jacobian<ValueMatrix, double, CoordinateSystemD, CoordinateSystemC, 2, 2, Dimensionless>;
  const Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 2, 2, Dimensionless> jacobianCB{1.0, 1.0, 1.0, -1.0};
  const RotationBD rotationBD{0.0, 1.0, -1.0, 0.0};
  const JacobianCD jacobianCD = jacobianCB * rotationBD;
  EXPECT_EQ(jacobianCD, (JacobianCD{-1.0, 1.0, 1.0, 1.0}));
  const ForceCovector<CoordinateSystemC> force{1.0, 2.0};
  EXPECT_EQ(jacobianCD.pullback(force), rotationBD.inverse().transform(jacobianCB.pullback(force)));
}

// The composition with a passive linear transformation with units: the change of basis L_AD from the coordinates (phi, r) in D to
// the polar coordinates (r, phi) in A, whose units are [q_i] / [p_j], gives the Jacobian of the Cartesian coordinates with respect
// to (phi, r): J_BD = J_BA L_AD = (-r sin(phi), cos(phi); r cos(phi), sin(phi)), in m / rad and 1. On the left, a scaling S_CB
// by 2 gives J_CA = S_CB J_BA.
TEST(TestJacobian, ComposeWithLinearTransformation) {  // NOLINT
  using AngleOverLength = Inverse<LengthOverAngle>::type;
  using LinearTransformationAD = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, 2, LengthOverAngle,
                                                             Dimensionless, Dimensionless, AngleOverLength>;
  using JacobianBD = Jacobian<ValueMatrix, double, CoordinateSystemD, CoordinateSystemB, 2, 2, LengthOverAngle, Dimensionless,
                              LengthOverAngle, Dimensionless>;
  const LinearTransformationAD linearTransformationAD{0.0, 1.0, 1.0, 0.0};
  const JacobianBD jacobianBD = jacobianBA * linearTransformationAD;
  EXPECT_EQ(jacobianBD, (JacobianBD{-2.0, 0.0, 0.0, 1.0}));
  const Vector<Quantities<ValueMatrix<double, 2, 1>, TypeMatrix<2, 1, AngularVelocity, Velocity>>, CoordinateSystemD> velocityInD{0.5, 1.0};
  EXPECT_EQ(jacobianBD.transform(velocityInD), jacobianBA.transform(linearTransformationAD.transform(velocityInD)));

  using ScalingCB = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 2, Dimensionless>;
  using JacobianCA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC, 2, 2, Dimensionless, LengthOverAngle,
                              Dimensionless, LengthOverAngle>;
  const ScalingCB scalingCB{2.0, 0.0, 0.0, 2.0};
  const JacobianCA jacobianCA = scalingCB * jacobianBA;
  EXPECT_EQ(jacobianCA, (JacobianCA{0.0, -4.0, 2.0, 0.0}));
  EXPECT_EQ(jacobianCA.transform(velocityInA), scalingCB.transform(jacobianBA.transform(velocityInA)));
}

// A Lorentz transformation is a change of basis between inertial frames, so it composes with Jacobians too: e.g. the Jacobian
// J_BA of (c t, x) in B with respect to the light cone coordinates (u, v) = (c t + x, c t - x) in A, followed by the reflection
// P_CB of x, is the Jacobian of (c t, -x), in which u and v swap their roles.
TEST(TestJacobian, ComposeWithLorentzTransformation) {  // NOLINT
  using LightConeJacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 2, 2, Dimensionless>;
  using LightConeJacobianCA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC, 2, 2, Dimensionless>;
  const LightConeJacobianBA lightConeJacobianBA{0.5, 0.5, 0.5, -0.5};
  const LorentzTransformation2<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC> reflectionCB{1.0, 0.0, 0.0, -1.0};
  const LightConeJacobianCA lightConeJacobianCA = reflectionCB * lightConeJacobianBA;
  EXPECT_EQ(lightConeJacobianCA, (LightConeJacobianCA{0.5, 0.5, -0.5, 0.5}));
  // On the right: the reflection P_BD of x before the Jacobian J_AB of (u, v) with respect to (c t, x), the inverse of J_BA, gives
  // J_AD = J_AB P_BD, i.e. u = c t - x and v = c t + x for the coordinates (c t, x) of D.
  using LightConeJacobianAB = Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA, 2, 2, Dimensionless>;
  using LightConeJacobianAD = Jacobian<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, 2, 2, Dimensionless>;
  const LightConeJacobianAB lightConeJacobianAB{1.0, 1.0, 1.0, -1.0};
  const LorentzTransformation2<ValueMatrix, double, CoordinateSystemD, CoordinateSystemB> reflectionBD{1.0, 0.0, 0.0, -1.0};
  const LightConeJacobianAD lightConeJacobianAD = lightConeJacobianAB * reflectionBD;
  EXPECT_EQ(lightConeJacobianAD, (LightConeJacobianAD{1.0, -1.0, 1.0, 1.0}));
  const SpacetimeVector2<ValueMatrix, double, CoordinateSystemD> displacement{1.0, 2.0};
  EXPECT_EQ(lightConeJacobianAD.transform(displacement), lightConeJacobianAB.transform(reflectionBD.transform(displacement)));
  EXPECT_EQ(lightConeJacobianAD.transform(displacement), (SpacetimeVector2<ValueMatrix, double, CoordinateSystemA>{-1.0, 3.0}));
}

// The inverse of a Jacobian is the Jacobian of the inverse coordinate change, J_AB = J_BA^-1, with the inverse units of the
// transposed matrix: that of the polar coordinates with respect to the Cartesian ones, (cos(phi), sin(phi); -sin(phi) / r,
// cos(phi) / r). It is the matrix inverse.
TEST(TestJacobian, Inverse) {  // NOLINT
  using AngleOverLength = Inverse<LengthOverAngle>::type;
  using JacobianAB = Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA, 2, 2, Dimensionless, Dimensionless,
                              AngleOverLength, AngleOverLength>;
  using JacobianAA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, 2, 2, Dimensionless, LengthOverAngle,
                              AngleOverLength, Dimensionless>;
  using JacobianBB = Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemB, 2, 2, Dimensionless>;
  const JacobianAB jacobianAB = jacobianBA.inverse();
  EXPECT_TRUE(jacobianAB.isApprox(JacobianAB{0.0, 1.0, -0.5, 0.0}, 1e-12));
  EXPECT_TRUE((jacobianAB * jacobianBA).isApprox(JacobianAA{1.0, 0.0, 0.0, 1.0}, 1e-12));
  EXPECT_TRUE((jacobianBA * jacobianAB).isApprox(JacobianBB{1.0, 0.0, 0.0, 1.0}, 1e-12));
  EXPECT_TRUE(jacobianAB.transform(jacobianBA.transform(velocityInA)).isApprox(velocityInA, 1e-12));
  const VelocityInB velocityInB{1.0, 2.0};
  EXPECT_TRUE(jacobianBA.transform(jacobianAB.transform(velocityInB)).isApprox(velocityInB, 1e-12));
}
