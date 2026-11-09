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

#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
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
#endif

using RotationBA = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
using RotationCB = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC>;
using RotationCA = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC>;

// Composition in the order of Drake's monogram notation, X_CB * X_BA = X_CA: first from A to B, then from B to C. The product of
// two rotations is a rotation, of the same type.
TEST(TestPassiveRotation, Compose) {                                          // NOLINT
  const RotationBA rotationBA{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const RotationCB rotationCB{0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0};  // Passive rotation of pi/2 around z.
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  const RotationCA rotationCA = rotationCB * rotationBA;
  EXPECT_EQ(rotationCA.transform(point), rotationCB.transform(rotationBA.transform(point)));
  // (1, 2, 3) is (1, 3, -2) in B and (3, -1, -2) in C.
  EXPECT_EQ(rotationCA.transform(point), (PositionPoint3<ValueMatrix, double, CoordinateSystemC>{3.0, -1.0, -2.0}));
  EXPECT_EQ(rotationCA, (RotationCA{0.0, 0.0, 1.0, -1.0, 0.0, 0.0, 0.0, -1.0, 0.0}));
}

// The inverse of a linear transformation is its matrix inverse, which Blitz++ does not provide.
#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
// The inverse transforms from To to From.
TEST(TestPassiveRotation, Inverse) {  // NOLINT
  using RotationAB = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA>;
  const RotationBA rotationBA{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  const RotationAB rotationAB = rotationBA.inverse();
  EXPECT_TRUE(rotationAB.isApprox(RotationAB{rotationBA.getQuantities().transpose()}, 1e-12));
  EXPECT_TRUE(rotationAB.transform(rotationBA.transform(point)).isApprox(point, 1e-12));
  const PositionPoint3<ValueMatrix, double, CoordinateSystemB> pointInB{1.0, 2.0, 3.0};
  EXPECT_TRUE(rotationBA.transform(rotationAB.transform(pointInB)).isApprox(pointInB, 1e-12));
  using RotationAA = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA>;
  using RotationBB = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemB>;
  EXPECT_TRUE((rotationAB * rotationBA).isApprox(RotationAA{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}, 1e-12));
  EXPECT_TRUE((rotationBA * rotationAB).isApprox(RotationBB{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}, 1e-12));
}
#endif
