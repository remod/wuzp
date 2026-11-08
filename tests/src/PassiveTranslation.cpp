#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "to_string.hpp"

using namespace wuzp;  // NOLINT

TEST(TestPassiveTranslation, StreamOperator) {  // NOLINT
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  const Transformation transformation{1.0, 2.0, 3.0};
  EXPECT_EQ(toString(transformation), toString(transformation.getTensor()));
}

TEST(TestPassiveTranslation, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 2.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 2, Length>>;
  using Object = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemB, Absolute>;
  const Object object{1.0, 4.0, 2.0, 5.0, 3.0, 6.0};
  const TransformedObject transformedObject{1.0, 4.0, 2.0, 5.0, 2.0, 5.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformPassiveTranslation) {  // NOLINT
  using Object = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD>;
  using TransformedObject = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformPassiveRotation) {  // NOLINT
  using Object = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD>;
  using TransformedObject = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
  const Object object{0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  const TransformedObject transformedObject{0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TranslationExpressedInTo) {  // NOLINT
  // From and To have the same axes, so a translation has the same components in both.
  using TranslationInFrom = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TranslationInTo = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA, CoordinateSystemB>;
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  EXPECT_EQ(TranslationInTo(0.0, 0.0, -1.0).transform(point), TranslationInFrom(0.0, 0.0, -1.0).transform(point));
}

// Transforming a point leaves the point unchanged, with every backend: the translation copies it, and a copy does not share
// the values of the original, also with Blitz++ (#60).
TEST(TestPassiveTranslation, TransformLeavesTheInputUnchanged) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  const TransformedObject transformedObject{1.0, 2.0, 2.0};
  EXPECT_EQ(transformation.transform(object), transformedObject);
  EXPECT_EQ(transformation.transform(object), transformedObject);
  EXPECT_EQ(object, (Object{1.0, 2.0, 3.0}));
}

// The object can be the result of an operation, here the sum of a point and a vector (#64).
TEST(TestPassiveTranslation, TransformSum) {  // NOLINT
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 1.0, 1.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(point + vector), (PositionPoint3<ValueMatrix, double, CoordinateSystemB>{2.0, 3.0, 3.0}));
}

// A transformation can be copied from a non-const object: the forwarding constructor does not hijack the copy (#66).
TEST(TestPassiveTranslation, CopyFromNonConstObject) {  // NOLINT
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  Transformation transformation{1.0, 2.0, 3.0};
  const Transformation copy(transformation);
  EXPECT_EQ(copy, transformation);
}
