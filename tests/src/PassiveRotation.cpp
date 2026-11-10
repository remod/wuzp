#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"
#include "wuzp/trafo/internal/validity.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "to_string.hpp"

using namespace wuzp;  // NOLINT

TEST(TestPassiveRotation, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -2.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveRotation, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -2.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveRotation, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -2.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveRotation, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 2, Length>>;
  using Object = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  using Transformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemB, Absolute>;
  const Object object{1.0, 4.0, 2.0, 5.0, 3.0, 6.0};
  const TransformedObject transformedObject{1.0, 4.0, 3.0, 6.0, -2.0, -5.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveRotation, TransformMassMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 3>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 3, Mass>>;
  using MassMatrix = Tensor<Indices<Down, Down>, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using VelocityVector = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using TransformedMassMatrix = Tensor<Indices<Down, Down>, MyQuantityMatrix, CoordinateSystemB, Relative>;
  using TransformedVelocityVector = VelocityVector3<ValueMatrix, double, CoordinateSystemB>;
  const MassMatrix massMatrix{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  const VelocityVector velocityVector{1.0, 2.0, 3.0};
  const QuantityD<Energy> energy{0.5 * innerProduct(innerProduct(massMatrix, velocityVector), velocityVector).getQuantity()};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const TransformedMassMatrix transformedMassMatrix{transformation.transform(massMatrix)};
  const TransformedVelocityVector transformedVelocityVector{transformation.transform(velocityVector)};
  const QuantityD<Energy> transformedEnergy{
      0.5 * innerProduct(innerProduct(transformedMassMatrix, transformedVelocityVector), transformedVelocityVector).getQuantity()};
  EXPECT_EQ(transformedEnergy, energy);
}

using RotationBA = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
using RotationCB = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC>;
using RotationCA = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC>;

// Composition in the order of Drake's monogram notation, X_CB * X_BA = X_CA: first from A to B, then from B to C. The product of
// two rotations is a rotation, of the same type.
TEST(TestPassiveRotation, Compose) {                                          // NOLINT
  const RotationBA rotationBA{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const RotationCB rotationCB{0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0};  // Passive rotation of pi/2 around z.
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  static_assert(std::is_same_v<decltype(rotationCB * rotationBA), RotationCA>);
  const RotationCA rotationCA = rotationCB * rotationBA;
  EXPECT_EQ(rotationCA.transform(point), rotationCB.transform(rotationBA.transform(point)));
  // (1, 2, 3) is (1, 3, -2) in B and (3, -1, -2) in C.
  EXPECT_EQ(rotationCA.transform(point), (PositionPoint3<ValueMatrix, double, CoordinateSystemC>{3.0, -1.0, -2.0}));
  EXPECT_EQ(rotationCA, (RotationCA{0.0, 0.0, 1.0, -1.0, 0.0, 0.0, 0.0, -1.0, 0.0}));
}

// The inverse transforms from To to From. It is the transpose, which every value matrix backend provides.
TEST(TestPassiveRotation, Inverse) {  // NOLINT
  using RotationAB = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA>;
  const RotationBA rotationBA{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  static_assert(std::is_same_v<decltype(rotationBA.inverse()), RotationAB>);
  const RotationAB rotationAB = rotationBA.inverse();
  EXPECT_EQ(rotationAB, RotationAB{rotationBA.getQuantities().transpose()});
  EXPECT_TRUE(rotationAB.transform(rotationBA.transform(point)).isApprox(point, 1e-12));
  const PositionPoint3<ValueMatrix, double, CoordinateSystemB> pointInB{1.0, 2.0, 3.0};
  EXPECT_TRUE(rotationBA.transform(rotationAB.transform(pointInB)).isApprox(pointInB, 1e-12));
  using RotationAA = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA>;
  using RotationBB = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemB>;
  EXPECT_TRUE((rotationAB * rotationBA).isApprox(RotationAA{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}, 1e-12));
  EXPECT_TRUE((rotationBA * rotationAB).isApprox(RotationBB{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}, 1e-12));
}

// A rotation transforms covectors and tensors with two lower indices with its inverse, the transpose: w' = w R^T and
// M' = R M R^T. It needs no matrix inverse, so it adds no rounding errors, and also works for integer matrices, whose matrix
// inverse is rejected.
TEST(TestPassiveRotation, TransformCovectorAndMassMatrixWithTranspose) {  // NOLINT
  using RotationIBA = PassiveSpatialRotation3<ValueMatrix, int, CoordinateSystemA, CoordinateSystemB>;
  using MassI = Quantities<ValueMatrix<int, 3, 3>, TypeMatrix<3, 3, Mass>>;
  const RotationIBA rotationI{1, 0, 0, 0, 0, 1, 0, -1, 0};  // Passive rotation of pi/2 around x.
  // The components of w R^T are the products of w with the rows of R: (1, 2, 3) gives (1, 3, -2).
  EXPECT_EQ(rotationI.transform(ForceCovector3<ValueMatrix, int, CoordinateSystemA>{1, 2, 3}),
            (ForceCovector3<ValueMatrix, int, CoordinateSystemB>{1, 3, -2}));
  // R swaps the rows and the columns of y and z, and negates the new rows and columns of z.
  EXPECT_EQ(rotationI.transform(Tensor<Indices<Down, Down>, MassI, CoordinateSystemA, Relative>{1, 2, 3, 2, 4, 5, 3, 5, 6}),
            (Tensor<Indices<Down, Down>, MassI, CoordinateSystemB, Relative>{1, 3, -2, 3, 6, -5, -2, -5, 4}));
  const RotationBA rotation = RotationBA::projectToRotation(1.01, 0.0, 0.0, 0.0, 0.02, 0.99, 0.0, -1.0, 0.0);
  const auto& matrix = rotation.getQuantities();
  const ForceCovector3<ValueMatrix, double, CoordinateSystemA> covector{1.0, 2.0, 3.0};
  EXPECT_EQ(rotation.transform(covector),
            (ForceCovector3<ValueMatrix, double, CoordinateSystemB>{innerProduct(covector.getQuantities(), matrix.transpose())}));
  using MassD = Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Mass>>;
  const Tensor<Indices<Down, Down>, MassD, CoordinateSystemA, Relative> massMatrix{1.0, 2.0, 3.0, 2.0, 4.0, 5.0, 3.0, 5.0, 6.0};
  EXPECT_EQ(rotation.transform(massMatrix), (Tensor<Indices<Down, Down>, MassD, CoordinateSystemB, Relative>{
                                                innerProduct(matrix, innerProduct(massMatrix.getQuantities(), matrix.transpose()))}));
}

// In debug builds, the constructors reject matrices that are not rotations, e.g. a shear, a scaling or a reflection. Release
// builds do not check. makeUnchecked() skips the check, and isValid() rejects them in every build.
TEST(TestPassiveRotation, RejectsMatricesThatAreNotRotationsInDebugBuilds) {  // NOLINT
  const auto shear = [] { return RotationBA{1.0, 5.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}; };
  const auto scaling = [] { return RotationBA{2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0}; };
  const auto reflection = [] { return RotationBA{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0}; };
  if constexpr (wuzp::internal::checksValidity) {
    EXPECT_THROW(shear(), std::invalid_argument);
    EXPECT_THROW(scaling(), std::invalid_argument);
    EXPECT_THROW(reflection(), std::invalid_argument);
  } else {
    EXPECT_NO_THROW(shear());
    EXPECT_NO_THROW(scaling());
    EXPECT_NO_THROW(reflection());
  }
  EXPECT_FALSE(RotationBA::makeUnchecked(1.0, 5.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0).isValid());
  EXPECT_FALSE(RotationBA::makeUnchecked(2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0).isValid());
  EXPECT_FALSE(RotationBA::makeUnchecked(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0).isValid());
  EXPECT_TRUE(RotationBA::makeUnchecked(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0).isValid());
  EXPECT_FALSE(RotationBA::makeUnchecked(1.0 + 1e-6, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0).isValid());
  EXPECT_TRUE(RotationBA::makeUnchecked(1.0 + 1e-6, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0).isValid(1e-5));
}

// The default constructor creates the identity: From and To have the same axes. A rotation is printed like its quantities.
TEST(TestPassiveRotation, DefaultIsIdentity) {                                // NOLINT
  const RotationBA rotationBA{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  EXPECT_EQ(RotationBA{}, (RotationBA{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}));
  EXPECT_NE(RotationBA{}, rotationBA);
  EXPECT_EQ(toString(rotationBA), toString(rotationBA.getQuantities()));
}

// The projection computes the rotation nearest to a matrix close to a rotation (see TestActiveRotation.ProjectToRotation).
TEST(TestPassiveRotation, ProjectToRotation) {                                // NOLINT
  const RotationBA rotationBA{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const RotationBA projected = RotationBA::projectToRotation(1.01, 0.0, 0.0, 0.0, 0.02, 0.99, 0.0, -1.0, 0.0);
  EXPECT_TRUE(projected.isValid(1e-14));
  EXPECT_TRUE(projected.isApprox(rotationBA, 0.05));
  EXPECT_THROW(RotationBA::projectToRotation(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0), std::invalid_argument);
}

// A rotation is a linear transformation: it composes with linear transformations into linear transformations, e.g. with a
// scaling, in both orders.
TEST(TestPassiveRotation, ComposeWithLinearTransformation) {  // NOLINT
  using ScalingCB = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 3, Dimensionless>;
  using ScalingAD = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, 3, Dimensionless>;
  using ScalingCA = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC, 3, Dimensionless>;
  using ScalingBD = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemD, CoordinateSystemB, 3, Dimensionless>;
  const RotationBA rotationBA{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const ScalingCB scalingCB{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.0};
  const ScalingAD scalingAD{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.0};
  static_assert(std::is_same_v<decltype(scalingCB * rotationBA), ScalingCA>);
  static_assert(std::is_same_v<decltype(rotationBA * scalingAD), ScalingBD>);
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  const PositionPoint3<ValueMatrix, double, CoordinateSystemD> pointInD{1.0, 2.0, 3.0};
  EXPECT_EQ((scalingCB * rotationBA).transform(point), scalingCB.transform(rotationBA.transform(point)));
  EXPECT_EQ((rotationBA * scalingAD).transform(pointInD), rotationBA.transform(scalingAD.transform(pointInD)));
  EXPECT_EQ((scalingCB * rotationBA), (ScalingCA{2.0, 0.0, 0.0, 0.0, 0.0, 4.0, 0.0, -8.0, 0.0}));
}
