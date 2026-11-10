#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "to_string.hpp"

using namespace wuzp;  // NOLINT

TEST(TestActiveTranslation, StreamOperator) {  // NOLINT
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> transformation{1.0, 2.0, 3.0};
  EXPECT_EQ(toString(transformation), toString(transformation.getTensor()));
}

TEST(TestActiveTranslation, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0};
  const Transformation transformation{0.0, 0.0, 1.0};  // Active translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveTranslation, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 4.0};
  const Transformation transformation{0.0, 0.0, 1.0};  // Active translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveTranslation, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0};
  const Transformation transformation{0.0, 0.0, 1.0};  // Active translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

// A translation leaves covectors, e.g. gradients, and tensors with two upper indices, e.g. covariances, unchanged, like vectors:
// their components do not depend on the origin.
TEST(TestActiveTranslation, TransformCovector) {  // NOLINT
  using Object = HomogeneousCovector<3, ValueMatrix, double, Force, CoordinateSystemA>;
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> transformation{0.0, 0.0,
                                                                                         1.0};  // Active translation of 1 along z.
  const Object object{1.0, 2.0, 3.0};
  EXPECT_EQ(transformation.transform(object), object);
}

TEST(TestActiveTranslation, TransformCovariance) {  // NOLINT
  using Object = Tensor<Indices<Up, Up>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Area>>, CoordinateSystemA, Relative>;
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> transformation{0.0, 0.0,
                                                                                         1.0};  // Active translation of 1 along z.
  const Object object{1.0, 0.5, 0.0, 0.5, 2.0, 0.0, 0.0, 0.0, 3.0};
  EXPECT_EQ(transformation.transform(object), object);
}

TEST(TestActiveTranslation, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 2, Length>>;
  using Object = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const Object object{1.0, 4.0, 2.0, 5.0, 3.0, 6.0};
  const TransformedObject transformedObject{1.0, 4.0, 2.0, 5.0, 4.0, 7.0};
  const Transformation transformation{0.0, 0.0, 1.0};  // Active translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveTranslation, TransformActiveTranslation) {  // NOLINT
  using Object = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0};
  const Transformation transformation{0.0, 0.0, 1.0};  // Active translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

// The conjugation T Q T^-1 of a rotation Q by a translation T is affine, x -> Q x + (I - Q) t, not the rotation Q (#72): composing
// with a proper rigid transformation that only rotates gives it. For t = (0, 0, 1) and a rotation by pi around x, the point
// (0, 0, 1) is mapped onto itself, while Q maps it to (0, 0, -1).
TEST(TestActiveTranslation, ConjugateRotation) {  // NOLINT
  using Translation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using RigidTransformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using Point = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  const Translation translation{0.0, 0.0, 1.0};                                                 // Active translation of 1 along z.
  const RigidTransformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0};  // Active rotation of pi around x.
  const RigidTransformation rotationOnly{rotation, Translation{0.0, 0.0, 0.0}};
  const Point point{0.0, 0.0, 1.0};
  const RigidTransformation conjugation = translation * rotationOnly * translation.inverse();
  EXPECT_EQ(conjugation.transform(point), point);
  EXPECT_EQ(conjugation.transform(point), translation.transform(rotation.transform(translation.inverse().transform(point))));
  EXPECT_EQ(rotation.transform(point), (Point{0.0, 0.0, -1.0}));
  // (I - Q) t = 2 t.
  EXPECT_EQ(conjugation.getRotation(), rotation);
  EXPECT_EQ(conjugation.getTranslation(), (Translation{0.0, 0.0, 2.0}));
}

TEST(TestActiveTranslation, Compose) {  // NOLINT
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  const Transformation first{0.0, 0.0, 1.0};
  const Transformation second{1.0, 2.0, 0.0};
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  const Transformation composition = second * first;
  EXPECT_EQ(composition, (Transformation{1.0, 2.0, 1.0}));
  EXPECT_EQ(composition.transform(point), second.transform(first.transform(point)));
  EXPECT_EQ(composition.transform(point), (PositionPoint3<ValueMatrix, double, CoordinateSystemA>{2.0, 4.0, 4.0}));
  // Vectors are not translated, neither by the composition.
  EXPECT_EQ(composition.transform(vector), second.transform(first.transform(vector)));
  EXPECT_EQ(composition.transform(vector), vector);
}

TEST(TestActiveTranslation, Inverse) {  // NOLINT
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  const Transformation transformation{1.0, 2.0, 3.0};
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{4.0, 5.0, 6.0};
  EXPECT_EQ(transformation.inverse(), (Transformation{-1.0, -2.0, -3.0}));
  EXPECT_EQ(transformation.inverse().transform(transformation.transform(point)), point);
  EXPECT_EQ(transformation.transform(transformation.inverse().transform(point)), point);
  EXPECT_EQ(transformation * transformation.inverse(), (Transformation{0.0, 0.0, 0.0}));
  EXPECT_EQ(transformation.inverse() * transformation, (Transformation{0.0, 0.0, 0.0}));
}

// Transforming a point leaves the point unchanged, with every backend: the translation copies it, and a copy does not share
// the values of the original, also with Blitz++ (#60).
TEST(TestActiveTranslation, TransformLeavesTheInputUnchanged) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const Transformation transformation{0.0, 0.0, 1.0};  // Active translation of 1 along z.
  const Object transformedObject{1.0, 2.0, 4.0};
  EXPECT_EQ(transformation.transform(object), transformedObject);
  EXPECT_EQ(transformation.transform(object), transformedObject);
  EXPECT_EQ(object, (Object{1.0, 2.0, 3.0}));
}

// The object can be the result of an operation, here the sum of a point and a vector (#64).
TEST(TestActiveTranslation, TransformSum) {  // NOLINT
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 1.0, 1.0};
  const Transformation transformation{0.0, 0.0, 1.0};  // Active translation of 1 along z.
  EXPECT_EQ(transformation.transform(point + vector), (PositionPoint3<ValueMatrix, double, CoordinateSystemA>{2.0, 3.0, 5.0}));
}

// A transformation can be copied from a non-const object: the forwarding constructor does not hijack the copy (#66).
TEST(TestActiveTranslation, CopyFromNonConstObject) {  // NOLINT
  ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> transformation{1.0, 2.0, 3.0};
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> copy(transformation);
  EXPECT_EQ(copy, transformation);
}

// A point is translated by adding the translation, so its components may have different units, e.g. a point in space and time.
TEST(TestActiveTranslation, TransformPointWithMixedUnits) {  // NOLINT
  using Object = Tensor<Indices<Up>, Quantities<ValueMatrix<double, 2, 1>, TypeMatrix<2, 1, Length, Time>>, CoordinateSystemA, Absolute>;
  using Transformation = ActiveTranslation<ValueMatrix, 2, double, CoordinateSystemA, Length, Time>;
  const Object object{1.0, 2.0};
  const Transformation transformation{3.0, 4.0};
  EXPECT_EQ(transformation.transform(object), (Object{4.0, 6.0}));
}

// isApprox is const and takes a precision, like that of tensors (#68).
TEST(TestActiveTranslation, IsApprox) {  // NOLINT
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  const Transformation transformation{1.0, 2.0, 3.0};
  EXPECT_TRUE(transformation.isApprox(Transformation{1.0, 2.0, 3.0 + 1e-14}));
  EXPECT_FALSE(transformation.isApprox(Transformation{1.0, 2.0, 4.0}));
  EXPECT_TRUE(transformation.isApprox(Transformation{1.0, 2.0, 3.001}, 1e-3));
  EXPECT_FALSE(transformation.isApprox(Transformation{1.0, 2.0, 3.001}, 1e-6));
}
