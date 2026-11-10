#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

TEST(TestPassiveProperRigidTransformation2, TransformPosition) {  // NOLINT
  //  const Position2<double, FromCoordinateSystem> object{1.0, 2.0};
  //  const Eigen::Quaterniond rotation{1.0, 0.0, 0.0, 0.0};
  //  const Translation2<double, CoordinateSystemB> translation{1.0, 2.0};
  //  const PassiveProperRigidTransformation2<FromCoordinateSystem, double> transformation{rotation, translation};
  //  const result = transformation.transform{object};
}

TEST(TestPassiveProperRigidTransformation3, TransformVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -2.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveProperRigidTransformation3, TransformPosition) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -3.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveProperRigidTransformation3, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -2.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

// The translation leaves covectors and tensors with two upper indices unchanged, so a rigid transformation only rotates them: a
// covector w into w R^T, with the same components as the vector, since R^-T = R, and a covariance S into R S R^T. The covector
// still pairs with the vectors like before.
TEST(TestPassiveProperRigidTransformation3, TransformCovector) {  // NOLINT
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  const Transformation transformation{rotation, translation};
  const ForceCovector3<ValueMatrix, double, CoordinateSystemA> force{1.0, 2.0, 3.0};
  const ForceCovector3<ValueMatrix, double, CoordinateSystemB> transformedForce = transformation.transform(force);
  EXPECT_EQ(transformedForce, (ForceCovector3<ValueMatrix, double, CoordinateSystemB>{1.0, 3.0, -2.0}));
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> displacement{4.0, 5.0, 6.0};
  EXPECT_EQ(innerProduct(transformedForce, transformation.transform(displacement)),
            castCoordinateSystem<CoordinateSystemB>(innerProduct(force, displacement)));
}

TEST(TestPassiveProperRigidTransformation3, TransformCovariance) {  // NOLINT
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;
  using QuantitiesT = Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Area>>;
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  const Transformation transformation{rotation, translation};
  const Tensor<Indices<Up, Up>, QuantitiesT, CoordinateSystemA, Relative> object{1.0, 0.5, 0.0, 0.5, 2.0, 0.0, 0.0, 0.0, 3.0};
  EXPECT_EQ(transformation.transform(object),
            (Tensor<Indices<Up, Up>, QuantitiesT, CoordinateSystemB, Relative>{1.0, 0.0, -0.5, 0.0, 3.0, 0.0, -0.5, 0.0, 2.0}));
}

TEST(TestPassiveProperRigidTransformation3, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 2, Length>>;
  using Object = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;
  using TransformedObject = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemB, Absolute>;
  const Object object{1.0, 4.0, 2.0, 5.0, 3.0, 6.0};
  const TransformedObject transformedObject{1.0, 4.0, 3.0, 6.0, -3.0, -6.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, -1.0};                          // Passive translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveProperRigidTransformation, TranslationIsExpressedInTo) {  // NOLINT
  // The rotation transforms into an intermediate coordinate system with the axes of To, so the translation is expressed in To.
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;
  EXPECT_TRUE((std::is_same_v<GetCoordinateSystem<Transformation::Translation::TensorT>::type, CoordinateSystemB>));
}

namespace {

using TransformationBA = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;
using TransformationCB = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemB, CoordinateSystemC, double>;
using TransformationCA = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemC, double>;
using TransformationAB = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemB, CoordinateSystemA, double>;
using PointInA = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;

// A passive rotation of pi/2 around x, and a passive translation of 1 along z.
const TransformationBA rigidBA{TransformationBA::Rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0},
                               TransformationBA::Translation{0.0, 0.0, -1.0}};
// A passive rotation of pi/2 around z, with the origin of B at (1, 0, 0) in C.
const TransformationCB rigidCB{TransformationCB::Rotation{0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
                               TransformationCB::Translation{1.0, 0.0, 0.0}};

}  // namespace

// Composition in the order of Drake's monogram notation, X_CB * X_BA = X_CA = (R_CB R_BA, R_CB t_BA + t_CB), like Drake's
// RigidTransform.
TEST(TestPassiveProperRigidTransformation3, Compose) {  // NOLINT
  const PointInA point{1.0, 2.0, 3.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  const TransformationCA rigidCA = rigidCB * rigidBA;
  EXPECT_EQ(rigidCA.transform(point), rigidCB.transform(rigidBA.transform(point)));
  // (1, 2, 3) in A is (1, 3, -2) + (0, 0, -1) in B, and (3, -1, -3) + (1, 0, 0) in C.
  EXPECT_EQ(rigidCA.transform(point), (PositionPoint3<ValueMatrix, double, CoordinateSystemC>{4.0, -1.0, -3.0}));
  // Vectors are only rotated: (1, 2, 3) in A is (1, 3, -2) in B, and (3, -1, -2) in C.
  EXPECT_EQ(rigidCA.transform(vector), rigidCB.transform(rigidBA.transform(vector)));
  EXPECT_EQ(rigidCA.transform(vector), (PositionVector3<ValueMatrix, double, CoordinateSystemC>{3.0, -1.0, -2.0}));
  EXPECT_EQ(rigidCA.getRotation().getQuantities(),
            (rigidCB.getRotation().getQuantities().innerProduct(rigidBA.getRotation().getQuantities())));
  // R_CB t_BA + t_CB = (0, 0, -1) + (1, 0, 0).
  EXPECT_EQ(rigidCA.getTranslation().getQuantities(), (TransformationCA::Translation{1.0, 0.0, -1.0}.getQuantities()));
}

TEST(TestPassiveProperRigidTransformation3, ComposeWithTranslation) {  // NOLINT
  const PointInA point{1.0, 2.0, 3.0};
  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA> translationAD{1.0, 2.0, 3.0};
  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC> translationCB{1.0, 2.0, 3.0};
  const PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemD, CoordinateSystemB, double> rigidBD = rigidBA * translationAD;
  const TransformationCA rigidCA = translationCB * rigidBA;
  const PositionPoint3<ValueMatrix, double, CoordinateSystemD> pointInD{1.0, 2.0, 3.0};
  EXPECT_EQ(rigidBD.transform(pointInD), rigidBA.transform(translationAD.transform(pointInD)));
  EXPECT_EQ(rigidCA.transform(point), translationCB.transform(rigidBA.transform(point)));
  // (1, 2, 3) in D is (2, 4, 6) in A, and (2, 6, -4) + (0, 0, -1) in B. (1, 2, 3) in A is (1, 3, -3) in B, and (2, 5, 0) in C.
  EXPECT_EQ(rigidBD.transform(pointInD), (PositionPoint3<ValueMatrix, double, CoordinateSystemB>{2.0, 6.0, -5.0}));
  EXPECT_EQ(rigidCA.transform(point), (PositionPoint3<ValueMatrix, double, CoordinateSystemC>{2.0, 5.0, 0.0}));
  // Vectors are only rotated.
  const PositionVector3<ValueMatrix, double, CoordinateSystemD> vectorInD{1.0, 2.0, 3.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  EXPECT_EQ(rigidBD.transform(vectorInD), rigidBA.transform(translationAD.transform(vectorInD)));
  EXPECT_EQ(rigidCA.transform(vector), translationCB.transform(rigidBA.transform(vector)));
  EXPECT_EQ(rigidBD.transform(vectorInD), (PositionVector3<ValueMatrix, double, CoordinateSystemB>{1.0, 3.0, -2.0}));
  EXPECT_EQ(rigidCA.transform(vector), (PositionVector3<ValueMatrix, double, CoordinateSystemC>{1.0, 3.0, -2.0}));
  // R t_AD + t_BA = (1, 3, -2) + (0, 0, -1), and t_BA + t_CB.
  EXPECT_EQ(rigidBD.getTranslation().getQuantities(), (TransformationCA::Translation{1.0, 3.0, -3.0}.getQuantities()));
  EXPECT_EQ(rigidCA.getTranslation().getQuantities(), (TransformationCA::Translation{1.0, 2.0, 2.0}.getQuantities()));
}

// The inverse X_AB = (R^T, -R^T t) only needs the transpose of the rotation, so it works with every value matrix backend. Its
// translation -R^T t is the vector from the origin of A to the origin of B, expressed in A, the To of the inverse.
TEST(TestPassiveProperRigidTransformation3, Inverse) {  // NOLINT
  const PointInA point{1.0, 2.0, 3.0};
  const TransformationAB rigidAB = rigidBA.inverse();
  EXPECT_EQ(rigidAB.getRotation().getQuantities(), rigidBA.getRotation().getQuantities().transpose());
  // -R^T t = -R^T (0, 0, -1).
  EXPECT_EQ(rigidAB.getTranslation().getTensor(), (PositionVector3<ValueMatrix, double, CoordinateSystemA>{0.0, -1.0, 0.0}));
  // The origin of B is at (0, -1, 0) in A.
  const PositionPoint3<ValueMatrix, double, CoordinateSystemB> originOfB{0.0, 0.0, 0.0};
  EXPECT_EQ(rigidAB.transform(originOfB), (PointInA{0.0, -1.0, 0.0}));
  EXPECT_EQ(rigidBA.transform(PointInA{0.0, -1.0, 0.0}), originOfB);
  EXPECT_EQ(rigidAB.transform(rigidBA.transform(point)), point);
  const PositionPoint3<ValueMatrix, double, CoordinateSystemB> pointInB{1.0, 2.0, 3.0};
  EXPECT_EQ(rigidBA.transform(rigidAB.transform(pointInB)), pointInB);
  using TransformationAA = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemA, double>;
  using TransformationBB = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemB, CoordinateSystemB, double>;
  const TransformationAA identity = rigidAB * rigidBA;
  EXPECT_EQ(identity.transform(point), point);
  EXPECT_EQ(identity.getRotation(), (TransformationAA::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}));
  EXPECT_EQ(identity.getTranslation(), (TransformationAA::Translation{0.0, 0.0, 0.0}));
  const TransformationBB identityInB = rigidBA * rigidAB;
  EXPECT_EQ(identityInB.getRotation(), (TransformationBB::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}));
  EXPECT_EQ(identityInB.getTranslation(), (TransformationBB::Translation{0.0, 0.0, 0.0}));
}

// isApprox is const, takes a precision, and compares both the rotation and the translation (#68).
TEST(TestPassiveProperRigidTransformation3, IsApprox) {  // NOLINT
  const TransformationBA almostEqual{rigidBA.getRotation(), TransformationBA::Translation{0.0, 0.0, -1.0 - 1e-14}};
  const TransformationBA otherRotation{TransformationBA::Rotation{rigidCB.getRotation().getQuantities()}, rigidBA.getTranslation()};
  const TransformationBA otherTranslation{rigidBA.getRotation(), TransformationBA::Translation{0.0, 0.0, -1.001}};
  EXPECT_TRUE(rigidBA.isApprox(almostEqual));
  EXPECT_FALSE(rigidBA.isApprox(otherRotation));
  EXPECT_FALSE(rigidBA.isApprox(otherTranslation));
  EXPECT_TRUE(rigidBA.isApprox(otherTranslation, 1e-2));
}

namespace {

using RotationBA = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
using RotationCB = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC>;
using PointInC = PositionPoint3<ValueMatrix, double, CoordinateSystemC>;

const RotationBA rotationBA{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
const RotationCB rotationCB{0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0};  // Passive rotation of pi/2 around z.

}  // namespace

// A rotation composes with a translation into a proper rigid transformation, in both orders: R_CB * t_BA = (R_CB, R_CB t_BA) and
// t_CB * R_BA = (R_BA, t_CB).
TEST(TestPassiveProperRigidTransformation3, ComposeRotationAndTranslation) {  // NOLINT
  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> translationBA{1.0, 2.0, 3.0};
  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC> translationCB{1.0, 2.0, 3.0};
  static_assert(std::is_same_v<decltype(rotationCB * translationBA), TransformationCA>);
  static_assert(std::is_same_v<decltype(translationCB * rotationBA), TransformationCA>);
  const TransformationCA translationFirst = rotationCB * translationBA;
  const TransformationCA translationLast = translationCB * rotationBA;
  const PointInA point{1.0, 2.0, 3.0};
  EXPECT_EQ(translationFirst.transform(point), rotationCB.transform(translationBA.transform(point)));
  EXPECT_EQ(translationLast.transform(point), translationCB.transform(rotationBA.transform(point)));
  // (1, 2, 3) in A is (2, 4, 6) in B and (4, -2, 6) in C; (1, 2, 3) in A is (1, 3, -2) in B and (2, 5, 1) in C.
  EXPECT_EQ(translationFirst.transform(point), (PointInC{4.0, -2.0, 6.0}));
  EXPECT_EQ(translationLast.transform(point), (PointInC{2.0, 5.0, 1.0}));
  EXPECT_EQ(translationFirst.getRotation().getQuantities(), rotationCB.getQuantities());
  EXPECT_EQ(translationFirst.getTranslation().getQuantities(), (TransformationCA::Translation{2.0, -1.0, 3.0}.getQuantities()));
  EXPECT_EQ(translationLast.getRotation().getQuantities(), rotationBA.getQuantities());
  EXPECT_EQ(translationLast.getTranslation().getQuantities(), translationCB.getQuantities());
}

// A proper rigid transformation composes with a rotation, in both orders: X_CB * R_BA = (R_CB R_BA, t_CB) and
// R_CB * X_BA = (R_CB R_BA, R_CB t_BA).
TEST(TestPassiveProperRigidTransformation3, ComposeWithRotation) {  // NOLINT
  static_assert(std::is_same_v<decltype(rigidCB * rotationBA), TransformationCA>);
  static_assert(std::is_same_v<decltype(rotationCB * rigidBA), TransformationCA>);
  const TransformationCA rotationFirst = rigidCB * rotationBA;
  const TransformationCA rotationLast = rotationCB * rigidBA;
  const PointInA point{1.0, 2.0, 3.0};
  EXPECT_EQ(rotationFirst.transform(point), rigidCB.transform(rotationBA.transform(point)));
  EXPECT_EQ(rotationLast.transform(point), rotationCB.transform(rigidBA.transform(point)));
  // (1, 2, 3) in A is (1, 3, -2) in B and (3, -1, -2) + (1, 0, 0) in C; (1, 2, 3) in A is (1, 3, -3) in B and (3, -1, -3) in C.
  EXPECT_EQ(rotationFirst.transform(point), (PointInC{4.0, -1.0, -2.0}));
  EXPECT_EQ(rotationLast.transform(point), (PointInC{3.0, -1.0, -3.0}));
  EXPECT_EQ(rotationFirst.getRotation().getQuantities(), (rotationCB * rotationBA).getQuantities());
  EXPECT_EQ(rotationFirst.getTranslation().getQuantities(), rigidCB.getTranslation().getQuantities());
  EXPECT_EQ(rotationLast.getRotation().getQuantities(), (rotationCB * rotationBA).getQuantities());
  // R_CB t_BA = R_CB (0, 0, -1).
  EXPECT_EQ(rotationLast.getTranslation().getQuantities(), (TransformationCA::Translation{0.0, 0.0, -1.0}.getQuantities()));
}
