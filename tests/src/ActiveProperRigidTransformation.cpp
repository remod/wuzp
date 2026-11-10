#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

TEST(TestActiveProperRigidTransformation2, TransformPosition) {  // NOLINT
  //  const Position2<double, FromCoordinateSystem> object{1.0, 2.0};
  //  const Eigen::Quaterniond rotation{1.0, 0.0, 0.0, 0.0};
  //  const Translation2<double, CoordinateSystemB> translation{1.0, 2.0};
  //  const ActiveProperRigidTransformation2<FromCoordinateSystem, CoordinateSystemB, double> transformation{rotation, translation};
  //  const result{transformation.transform{object}};
}

TEST(TestActiveProperRigidTransformation3, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, -3.0, 2.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, 1.0};                           // Active translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveProperRigidTransformation3, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, -3.0, 3.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, 1.0};                           // Active translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveProperRigidTransformation3, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, -3.0, 2.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, 1.0};                           // Active translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveProperRigidTransformation3, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 2, Length>>;
  using Object = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  using Transformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using TransformedObject = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const Object object{1.0, 4.0, 2.0, 5.0, 3.0, 6.0};
  const TransformedObject transformedObject{1.0, 4.0, -3.0, -6.0, 3.0, 6.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, 1.0};                           // Active translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveProperRigidTransformation3, TransformActiveTranslation) {  // NOLINT
  using Object = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using TransformedObject = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, -3.0, 2.0};
  const Transformation::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  const Transformation::Translation translation{0.0, 0.0, 1.0};                           // Active translation of 1 along z.
  const Transformation transformation{rotation, translation};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

namespace {

using Transformation3 = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
using Point3 = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
using Vector3 = PositionVector3<ValueMatrix, double, CoordinateSystemA>;

// A rotation of pi/2 around x, then a translation of 1 along z.
const Transformation3 rigid{Transformation3::Rotation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0},
                            Transformation3::Translation{0.0, 0.0, 1.0}};
// A rotation of pi/2 around z, then a translation of 1 along x.
const Transformation3 otherRigid{Transformation3::Rotation{0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0},
                                 Transformation3::Translation{1.0, 0.0, 0.0}};

}  // namespace

// The composition applies the right-hand side first: (R_lhs R_rhs, R_lhs t_rhs + t_lhs).
TEST(TestActiveProperRigidTransformation3, Compose) {  // NOLINT
  const Point3 point{1.0, 2.0, 3.0};
  const Vector3 vector{1.0, 2.0, 3.0};
  const Transformation3 composition = otherRigid * rigid;
  EXPECT_EQ(composition.transform(point), otherRigid.transform(rigid.transform(point)));
  // (1, 2, 3) is moved to (1, -3, 3) by rigid, and then to (3, 1, 3) + (1, 0, 0) by otherRigid.
  EXPECT_EQ(composition.transform(point), (Point3{4.0, 1.0, 3.0}));
  // Vectors are only rotated: (1, 2, 3) is rotated to (1, -3, 2), and then to (3, 1, 2).
  EXPECT_EQ(composition.transform(vector), otherRigid.transform(rigid.transform(vector)));
  EXPECT_EQ(composition.transform(vector), (Vector3{3.0, 1.0, 2.0}));
  EXPECT_EQ(composition.getRotation(), otherRigid.getRotation() * rigid.getRotation());
  // R_lhs t_rhs + t_lhs = (0, 0, 1) + (1, 0, 0).
  EXPECT_EQ(composition.getTranslation(), (Transformation3::Translation{1.0, 0.0, 1.0}));
}

TEST(TestActiveProperRigidTransformation3, ComposeWithTranslation) {  // NOLINT
  const Point3 point{1.0, 2.0, 3.0};
  const Transformation3::Translation translation{1.0, 2.0, 3.0};
  const Transformation3 translationFirst = rigid * translation;
  const Transformation3 translationLast = translation * rigid;
  EXPECT_EQ(translationFirst.transform(point), rigid.transform(translation.transform(point)));
  EXPECT_EQ(translationLast.transform(point), translation.transform(rigid.transform(point)));
  // (1, 2, 3) + t = (2, 4, 6) is moved to (2, -6, 4) + (0, 0, 1), and (1, -3, 3) to (1, -3, 3) + t.
  EXPECT_EQ(translationFirst.transform(point), (Point3{2.0, -6.0, 5.0}));
  EXPECT_EQ(translationLast.transform(point), (Point3{2.0, -1.0, 6.0}));
  // Vectors are only rotated.
  const Vector3 vector{1.0, 2.0, 3.0};
  EXPECT_EQ(translationFirst.transform(vector), rigid.transform(translation.transform(vector)));
  EXPECT_EQ(translationLast.transform(vector), translation.transform(rigid.transform(vector)));
  EXPECT_EQ(translationFirst.transform(vector), (Vector3{1.0, -3.0, 2.0}));
  EXPECT_EQ(translationLast.transform(vector), (Vector3{1.0, -3.0, 2.0}));
  // R t + t_rigid = (1, -3, 2) + (0, 0, 1), and t_rigid + t.
  EXPECT_EQ(translationFirst.getTranslation(), (Transformation3::Translation{1.0, -3.0, 3.0}));
  EXPECT_EQ(translationLast.getTranslation(), (Transformation3::Translation{1.0, 2.0, 4.0}));
}

// The inverse (R^T, -R^T t) only needs the transpose of the rotation, so it works with every value matrix backend.
TEST(TestActiveProperRigidTransformation3, Inverse) {  // NOLINT
  const Point3 point{1.0, 2.0, 3.0};
  const Transformation3 inverse = rigid.inverse();
  EXPECT_EQ(inverse.getRotation(), Transformation3::Rotation{rigid.getRotation().getQuantities().transpose()});
  // -R^T t = -R^T (0, 0, 1).
  EXPECT_EQ(inverse.getTranslation(), (Transformation3::Translation{0.0, -1.0, 0.0}));
  EXPECT_EQ(inverse.transform(rigid.transform(point)), point);
  EXPECT_EQ(rigid.transform(inverse.transform(point)), point);
  const Transformation3::Rotation identityRotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  const Transformation3::Translation zeroTranslation{0.0, 0.0, 0.0};
  const Transformation3 inverseFirst = inverse * rigid;
  const Transformation3 inverseLast = rigid * inverse;
  EXPECT_EQ(inverseFirst.getRotation(), identityRotation);
  EXPECT_EQ(inverseFirst.getTranslation(), zeroTranslation);
  EXPECT_EQ(inverseLast.getRotation(), identityRotation);
  EXPECT_EQ(inverseLast.getTranslation(), zeroTranslation);
}

// The conjugation G Q G^-1 of a rotation Q by a proper rigid transformation G is affine, x -> R Q R^T x + (I - R Q R^T) t, not
// the rotation R Q R^T (#72): composing with a proper rigid transformation that only rotates gives it.
TEST(TestActiveProperRigidTransformation3, ConjugateRotation) {                            // NOLINT
  const Transformation3::Rotation rotation{0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};  // Active rotation of pi/2 around z.
  const Transformation3 rotationOnly{rotation, Transformation3::Translation{0.0, 0.0, 0.0}};
  const Transformation3 conjugation = rigid * rotationOnly * rigid.inverse();
  const Point3 point{1.0, 2.0, 3.0};
  EXPECT_EQ(conjugation.transform(point), rigid.transform(rotation.transform(rigid.inverse().transform(point))));
  // The rotation of pi/2 around the image of z, -y, around the axis through the image of the origin, t = (0, 0, 1).
  EXPECT_EQ(conjugation.getRotation(), (Transformation3::Rotation{0.0, 0.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0}));
  EXPECT_EQ(conjugation.transform(Point3{0.0, 5.0, 1.0}), (Point3{0.0, 5.0, 1.0}));
}

// isApprox is const, takes a precision, and compares both the rotation and the translation (#68).
TEST(TestActiveProperRigidTransformation3, IsApprox) {  // NOLINT
  const Transformation3 almostEqual{rigid.getRotation(), Transformation3::Translation{0.0, 0.0, 1.0 + 1e-14}};
  const Transformation3 otherRotation{otherRigid.getRotation(), rigid.getTranslation()};
  const Transformation3 otherTranslation{rigid.getRotation(), Transformation3::Translation{0.0, 0.0, 1.001}};
  EXPECT_TRUE(rigid.isApprox(almostEqual));
  EXPECT_FALSE(rigid.isApprox(otherRotation));
  EXPECT_FALSE(rigid.isApprox(otherTranslation));
  EXPECT_TRUE(rigid.isApprox(otherTranslation, 1e-2));
}

// A rotation composes with a translation into a proper rigid transformation, in both orders: R * t = (R, R t) and
// t * R = (R, t).
TEST(TestActiveProperRigidTransformation3, ComposeRotationAndTranslation) {                // NOLINT
  const Transformation3::Rotation rotation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  const Transformation3::Translation translation{1.0, 2.0, 3.0};
  static_assert(std::is_same_v<decltype(rotation * translation), Transformation3>);
  static_assert(std::is_same_v<decltype(translation * rotation), Transformation3>);
  const Transformation3 translationFirst = rotation * translation;
  const Transformation3 translationLast = translation * rotation;
  const Point3 point{1.0, 2.0, 3.0};
  EXPECT_EQ(translationFirst.transform(point), rotation.transform(translation.transform(point)));
  EXPECT_EQ(translationLast.transform(point), translation.transform(rotation.transform(point)));
  // (1, 2, 3) + t = (2, 4, 6) is rotated to (2, -6, 4), and (1, 2, 3) is rotated to (1, -3, 2), then moved to (2, -1, 5).
  EXPECT_EQ(translationFirst.transform(point), (Point3{2.0, -6.0, 4.0}));
  EXPECT_EQ(translationLast.transform(point), (Point3{2.0, -1.0, 5.0}));
  EXPECT_EQ(translationFirst.getRotation(), rotation);
  EXPECT_EQ(translationFirst.getTranslation(), (Transformation3::Translation{1.0, -3.0, 2.0}));
  EXPECT_EQ(translationLast.getRotation(), rotation);
  EXPECT_EQ(translationLast.getTranslation(), translation);
  // Vectors are only rotated.
  const Vector3 vector{1.0, 2.0, 3.0};
  EXPECT_EQ(translationFirst.transform(vector), (Vector3{1.0, -3.0, 2.0}));
  EXPECT_EQ(translationLast.transform(vector), (Vector3{1.0, -3.0, 2.0}));
}

// A proper rigid transformation composes with a rotation, in both orders: (R, t) * Q = (R Q, t) and Q * (R, t) = (Q R, Q t).
TEST(TestActiveProperRigidTransformation3, ComposeWithRotation) {                          // NOLINT
  const Transformation3::Rotation rotation{0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};  // Active rotation of pi/2 around z.
  static_assert(std::is_same_v<decltype(rigid * rotation), Transformation3>);
  static_assert(std::is_same_v<decltype(rotation * rigid), Transformation3>);
  const Transformation3 rotationFirst = rigid * rotation;
  const Transformation3 rotationLast = rotation * rigid;
  const Point3 point{1.0, 2.0, 3.0};
  EXPECT_EQ(rotationFirst.transform(point), rigid.transform(rotation.transform(point)));
  EXPECT_EQ(rotationLast.transform(point), rotation.transform(rigid.transform(point)));
  // (1, 2, 3) is rotated to (-2, 1, 3), then moved to (-2, -3, 2); (1, 2, 3) is moved to (1, -3, 3), then rotated to (3, 1, 3).
  EXPECT_EQ(rotationFirst.transform(point), (Point3{-2.0, -3.0, 2.0}));
  EXPECT_EQ(rotationLast.transform(point), (Point3{3.0, 1.0, 3.0}));
  EXPECT_EQ(rotationFirst.getRotation(), rigid.getRotation() * rotation);
  EXPECT_EQ(rotationFirst.getTranslation(), rigid.getTranslation());
  EXPECT_EQ(rotationLast.getRotation(), rotation * rigid.getRotation());
  // Q t = (0, 0, 1).
  EXPECT_EQ(rotationLast.getTranslation(), (Transformation3::Translation{0.0, 0.0, 1.0}));
}
