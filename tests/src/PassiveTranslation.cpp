#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "to_string.hpp"

using namespace wuzp;  // NOLINT

TEST(TestPassiveTranslation, StreamOperator) {  // NOLINT
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  const Transformation transformation{1.0, 2.0, 3.0};
  EXPECT_EQ(toString(transformation), toString(transformation.getTensor()));
}

TEST(TestPassiveTranslation, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 2.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 2, Length>>;
  using Object = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemB, Absolute>;
  const Object object{1.0, 4.0, 2.0, 5.0, 3.0, 6.0};
  const TransformedObject transformedObject{1.0, 4.0, 2.0, 5.0, 2.0, 5.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

// The vector of the translation is the vector from the origin of To to the origin of From, expressed in To, which has the axes
// of From: the origin of A is at (0, 0, -1) in B.
TEST(TestPassiveTranslation, TranslationExpressedInTo) {  // NOLINT
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  static_assert(std::is_same_v<GetCoordinateSystem<Transformation::TensorT>::type, CoordinateSystemB>);
  const Transformation transformation{0.0, 0.0, -1.0};
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> originOfA{0.0, 0.0, 0.0};
  EXPECT_EQ(makeRelative(transformation.transform(originOfA)), transformation.getTensor());
}

// Composition in the order of Drake's monogram notation, X_CB * X_BA = X_CA: p_CoAo_C = p_CoBo_C + p_BoAo_B, since A, B and C
// have the same axes.
TEST(TestPassiveTranslation, Compose) {  // NOLINT
  using TranslationBA = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using TranslationCB = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC>;
  using TranslationCA = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC>;
  const TranslationBA translationBA{0.0, 0.0, -1.0};
  const TranslationCB translationCB{1.0, 2.0, 0.0};
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  const TranslationCA translationCA = translationCB * translationBA;
  EXPECT_EQ(translationCA, (TranslationCA{1.0, 2.0, -1.0}));
  EXPECT_EQ(translationCA.transform(point), translationCB.transform(translationBA.transform(point)));
  EXPECT_EQ(translationCA.transform(point), (PositionPoint3<ValueMatrix, double, CoordinateSystemC>{2.0, 4.0, 2.0}));
  // Vectors keep their components, also through the composition.
  EXPECT_EQ(translationCA.transform(vector), translationCB.transform(translationBA.transform(vector)));
  EXPECT_EQ(translationCA.transform(vector), (PositionVector3<ValueMatrix, double, CoordinateSystemC>{1.0, 2.0, 3.0}));
}

// The inverse transforms from To to From, with the vector from the origin of From to the origin of To, -t, expressed in From.
TEST(TestPassiveTranslation, Inverse) {  // NOLINT
  using TranslationBA = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  using TranslationAB = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA>;
  const TranslationBA translationBA{1.0, 2.0, 3.0};
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{4.0, 5.0, 6.0};
  const TranslationAB translationAB = translationBA.inverse();
  EXPECT_EQ(translationAB, (TranslationAB{-1.0, -2.0, -3.0}));
  EXPECT_EQ(translationAB.transform(translationBA.transform(point)), point);
  const PositionPoint3<ValueMatrix, double, CoordinateSystemB> pointInB{4.0, 5.0, 6.0};
  EXPECT_EQ(translationBA.transform(translationAB.transform(pointInB)), pointInB);
  using TranslationAA = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA>;
  using TranslationBB = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemB>;
  EXPECT_EQ(translationAB * translationBA, (TranslationAA{0.0, 0.0, 0.0}));
  EXPECT_EQ(translationBA * translationAB, (TranslationBB{0.0, 0.0, 0.0}));
}

// Transforming a point leaves the point unchanged, with every backend: the translation copies it, and a copy does not share
// the values of the original, also with Blitz++ (#60).
TEST(TestPassiveTranslation, TransformLeavesTheInputUnchanged) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
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
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 1.0, 1.0};
  const Transformation transformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(point + vector), (PositionPoint3<ValueMatrix, double, CoordinateSystemB>{2.0, 3.0, 3.0}));
}

// A transformation can be copied from a non-const object: the forwarding constructor does not hijack the copy (#66).
TEST(TestPassiveTranslation, CopyFromNonConstObject) {  // NOLINT
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  Transformation transformation{1.0, 2.0, 3.0};
  const Transformation copy(transformation);
  EXPECT_EQ(copy, transformation);
}

// isApprox is const and takes a precision, like that of tensors (#68).
TEST(TestPassiveTranslation, IsApprox) {  // NOLINT
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  const Transformation transformation{1.0, 2.0, 3.0};
  EXPECT_TRUE(transformation.isApprox(Transformation{1.0, 2.0, 3.0 + 1e-14}));
  EXPECT_FALSE(transformation.isApprox(Transformation{1.0, 2.0, 4.0}));
  EXPECT_TRUE(transformation.isApprox(Transformation{1.0, 2.0, 3.001}, 1e-3));
  EXPECT_FALSE(transformation.isApprox(Transformation{1.0, 2.0, 3.001}, 1e-6));
}
