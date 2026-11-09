#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

TEST(TestPassiveRotation, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -2.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveRotation, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -2.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveRotation, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
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
  using Transformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemB, Absolute>;
  const Object object{1.0, 4.0, 2.0, 5.0, 3.0, 6.0};
  const TransformedObject transformedObject{1.0, 4.0, 3.0, 6.0, -2.0, -5.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestPassiveRotation, TransformMassMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 3>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 3, Mass>>;
  using MassMatrix = Tensor<Indices<Down, Down>, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using VelocityVector = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
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
#endif

TEST(TestPassiveRotation, TransformPassiveTranslation) {  // NOLINT
  using Object = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
  using Transformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD>;
  using TransformedObject = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 3.0, -2.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

// TEST(TestPassiveRotation, TransformPassiveRotation) {  // NOLINT
//  using Object = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
//  using Transformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD>;
//  using TransformedObject = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
//  const double sqrt2_2 = 0.5 * std::sqrt{2.0};
//  const Object object{0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
//  const TransformedObject transformedObject{0.0, sqrt2_2, sqrt2_2, sqrt2_2, 0.5, -0.5, sqrt2_2, -0.5, 0.5};
//  const Transformation transformation{Eigen::AngleAxisd{M_PI_2, Eigen::Vector3d{1.0, 0.0, 0.0}}.toRotationMatrix()};
//  EXPECT_TRUE(transformation.transform(object).isApprox(transformedObject));
//  // TODO(remod): Test converting an object with original and transformed transformation, and check whether they match.
//}
