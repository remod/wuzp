#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

TEST(TestActiveRotation, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, -3.0, 2.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveRotation, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, -3.0, 2.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveRotation, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, -3.0, 2.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveRotation, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 2, Length>>;
  using Object = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const Object object{1.0, 4.0, 2.0, 5.0, 3.0, 6.0};
  const TransformedObject transformedObject{1.0, 4.0, -3.0, -6.0, 2.0, 5.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestActiveRotation, TransformMassMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 3>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 3, Mass>>;
  using MassMatrix = Tensor<Indices<Down, Down>, MyQuantityMatrix, CoordinateSystemA, Relative>;
  using VelocityVector = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  const MassMatrix massMatrix{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  const VelocityVector velocityVector{1.0, 2.0, 3.0};
  const QuantityD<Energy> energy{0.5 * innerProduct(innerProduct(massMatrix, velocityVector), velocityVector).getQuantity()};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  const MassMatrix transformedMassMatrix{transformation.transform(massMatrix)};
  const VelocityVector transformedVelocityVector{transformation.transform(velocityVector)};
  const QuantityD<Energy> transformedEnergy{
      0.5 * innerProduct(innerProduct(transformedMassMatrix, transformedVelocityVector), transformedVelocityVector).getQuantity()};
  EXPECT_EQ(transformedEnergy, energy);
}
#endif

TEST(TestActiveRotation, TransformActiveTranslation) {  // NOLINT
  using Object = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, -3.0, 2.0};
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

// The composition applies the right-hand side first: two rotations by pi/2 around x are a rotation by pi around x.
TEST(TestActiveRotation, Compose) {  // NOLINT
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  const Transformation aroundX{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  const Transformation aroundZ{0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};  // Active rotation of pi/2 around z.
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  // The product of two rotations is a rotation, of the same type.
  const Transformation twiceAroundX = aroundX * aroundX;
  EXPECT_EQ(twiceAroundX, (Transformation{1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0}));
  EXPECT_EQ((aroundZ * aroundX).transform(point), aroundZ.transform(aroundX.transform(point)));
  EXPECT_EQ((aroundZ * aroundX).transform(point), (PositionPoint3<ValueMatrix, double, CoordinateSystemA>{3.0, 1.0, 2.0}));
  EXPECT_NE(aroundZ * aroundX, aroundX * aroundZ);
}

// The inverse of a linear transformation is its matrix inverse, which Blitz++ does not provide.
#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestActiveRotation, Inverse) {  // NOLINT
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  const Transformation inverse = transformation.inverse();
  EXPECT_TRUE(inverse.isApprox(Transformation{transformation.getQuantities().transpose()}, 1e-12));
  EXPECT_TRUE((inverse * transformation).isApprox(Transformation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}, 1e-12));
  EXPECT_TRUE((transformation * inverse).isApprox(Transformation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}, 1e-12));
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  EXPECT_TRUE(inverse.transform(transformation.transform(point)).isApprox(point, 1e-12));
  EXPECT_TRUE(transformation.transform(inverse.transform(point)).isApprox(point, 1e-12));
}

// The conjugation R Q R^-1 of a rotation Q by a rotation R is the composition of R, Q and the inverse of R.
TEST(TestActiveRotation, TransformActiveRotation) {  // NOLINT
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  const Transformation object{0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};          // Active rotation of pi/2 around z.
  // The rotation of pi/2 around the image of z, -y.
  const Transformation transformedObject{0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0};
  EXPECT_TRUE(transformation.transform(object).isApprox(transformedObject, 1e-12));
  EXPECT_TRUE((transformation * object * transformation.inverse()).isApprox(transformedObject, 1e-12));
}
#endif
