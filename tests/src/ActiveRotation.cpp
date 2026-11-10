#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveLinearTransformation.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"
#include "wuzp/trafo/internal/validity.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "to_string.hpp"

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
  static_assert(std::is_same_v<decltype(aroundX * aroundX), Transformation>);
  const Transformation twiceAroundX = aroundX * aroundX;
  EXPECT_EQ(twiceAroundX, (Transformation{1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0}));
  EXPECT_EQ((aroundZ * aroundX).transform(point), aroundZ.transform(aroundX.transform(point)));
  EXPECT_EQ((aroundZ * aroundX).transform(point), (PositionPoint3<ValueMatrix, double, CoordinateSystemA>{3.0, 1.0, 2.0}));
  EXPECT_NE(aroundZ * aroundX, aroundX * aroundZ);
}

// The inverse of a rotation is its transpose, which every value matrix backend provides.
TEST(TestActiveRotation, Inverse) {  // NOLINT
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  const Transformation transformation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  const Transformation inverse = transformation.inverse();
  EXPECT_EQ(inverse, Transformation{transformation.getQuantities().transpose()});
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

namespace {

using Rotation3 = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;

// The rotation by an angle around z.
Rotation3 rotationAroundZ(const double angle) {
  return Rotation3{std::cos(angle), -std::sin(angle), 0.0, std::sin(angle), std::cos(angle), 0.0, 0.0, 0.0, 1.0};
}

// The rotation by an angle around x.
Rotation3 rotationAroundX(const double angle) {
  return Rotation3{1.0, 0.0, 0.0, 0.0, std::cos(angle), -std::sin(angle), 0.0, std::sin(angle), std::cos(angle)};
}

}  // namespace

// A rotation transforms covectors with its inverse, the transpose: w' = w R^-1 = w R^T. It needs no matrix inverse, so it adds no
// rounding errors, and also works for integer matrices, whose matrix inverse is rejected.
TEST(TestActiveRotation, TransformCovector) {  // NOLINT
  using CovectorI = HomogeneousCovector<3, ValueMatrix, int, Force, CoordinateSystemA>;
  using RotationI = ActiveSpatialRotation3<ValueMatrix, int, CoordinateSystemA>;
  const RotationI rotationI{1, 0, 0, 0, 0, -1, 0, 1, 0};  // Active rotation of pi/2 around x.
  // The components of w R^T are the products of w with the rows of R: (1, 2, 3) gives (1, -3, 2).
  EXPECT_EQ(rotationI.transform(CovectorI{1, 2, 3}), (CovectorI{1, -3, 2}));
  using Covector = HomogeneousCovector<3, ValueMatrix, double, Force, CoordinateSystemA>;
  const Rotation3 rotation = rotationAroundZ(0.5) * rotationAroundX(0.3);
  const Covector covector{1.0, 2.0, 3.0};
  EXPECT_EQ(rotation.transform(covector), Covector{innerProduct(covector.getQuantities(), rotation.getQuantities().transpose())});
  // The pairing of a covector and a vector is invariant.
  const VelocityVector3<ValueMatrix, double, CoordinateSystemA> velocity{4.0, 5.0, 6.0};
  EXPECT_TRUE(innerProduct(rotation.transform(covector), rotation.transform(velocity)).isApprox(innerProduct(covector, velocity), 1e-14));
}

// A rotation transforms a tensor with two lower indices, e.g. a mass matrix, with its transpose too: M' = R^-T M R^-1 = R M R^T.
// It conjugates a linear transformation A into R A R^T, like a linear map, also for integer matrices.
TEST(TestActiveRotation, TransformMassMatrixAndLinearTransformationWithTranspose) {  // NOLINT
  using MassMatrixI = Tensor<Indices<Down, Down>, Quantities<ValueMatrix<int, 3, 3>, TypeMatrix<3, 3, Mass>>, CoordinateSystemA, Relative>;
  using LinearTransformationI = ActiveLinearTransformation<ValueMatrix, int, CoordinateSystemA, 3, 3, Dimensionless>;
  using RotationI = ActiveSpatialRotation3<ValueMatrix, int, CoordinateSystemA>;
  const RotationI rotationI{1, 0, 0, 0, 0, -1, 0, 1, 0};  // Active rotation of pi/2 around x.
  // R swaps the rows and the columns of y and z, and negates the new rows and columns of y.
  EXPECT_EQ(rotationI.transform(MassMatrixI{1, 2, 3, 2, 4, 5, 3, 5, 6}), (MassMatrixI{1, -3, 2, -3, 6, -5, 2, -5, 4}));
  EXPECT_EQ(rotationI.transform(LinearTransformationI{1, 2, 0, 0, 3, 0, 0, 0, 4}), (LinearTransformationI{1, 0, 2, 0, 4, 0, 0, 0, 3}));
  using MassMatrix =
      Tensor<Indices<Down, Down>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Mass>>, CoordinateSystemA, Relative>;
  const Rotation3 rotation = rotationAroundZ(0.5) * rotationAroundX(0.3);
  const MassMatrix massMatrix{1.0, 2.0, 3.0, 2.0, 4.0, 5.0, 3.0, 5.0, 6.0};
  const auto& matrix = rotation.getQuantities();
  EXPECT_EQ(rotation.transform(massMatrix), MassMatrix{innerProduct(matrix, innerProduct(massMatrix.getQuantities(), matrix.transpose()))});
}

// In debug builds, the constructors reject matrices that are not rotations: a shear, which stretches the unit vector along y to
// a length of 5.1, a scaling, and a reflection, which is orthogonal, but has the determinant -1. Release builds do not check.
TEST(TestActiveRotation, RejectsMatricesThatAreNotRotationsInDebugBuilds) {  // NOLINT
  const auto shear = [] { return Rotation3{1.0, 5.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}; };
  const auto scaling = [] { return Rotation3{2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0}; };
  const auto reflection = [] { return Rotation3{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0}; };
  if constexpr (wuzp::internal::checksValidity) {
    EXPECT_THROW(shear(), std::invalid_argument);
    EXPECT_THROW(scaling(), std::invalid_argument);
    EXPECT_THROW(reflection(), std::invalid_argument);
  } else {
    EXPECT_NO_THROW(shear());
    EXPECT_NO_THROW(scaling());
    EXPECT_NO_THROW(reflection());
  }
  EXPECT_NO_THROW(rotationAroundZ(0.5));
}

// makeUnchecked() skips the check, also in debug builds, and isValid() tells whether a matrix is a rotation, in every build.
TEST(TestActiveRotation, MakeUnchecked) {  // NOLINT
  EXPECT_NO_THROW(Rotation3::makeUnchecked(1.0, 5.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0));
  const Rotation3 shear = Rotation3::makeUnchecked(1.0, 5.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
  EXPECT_FALSE(shear.isValid());
  EXPECT_FALSE(Rotation3::makeUnchecked(2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0).isValid());
  EXPECT_FALSE(Rotation3::makeUnchecked(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0).isValid());
  EXPECT_TRUE(Rotation3::makeUnchecked(rotationAroundZ(0.5).getQuantities()).isValid());
  EXPECT_EQ(Rotation3::makeUnchecked(rotationAroundZ(0.5).getQuantities()), rotationAroundZ(0.5));
}

// The default constructor creates the identity.
TEST(TestActiveRotation, DefaultIsIdentity) {  // NOLINT
  EXPECT_EQ(Rotation3{}, (Rotation3{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}));
  EXPECT_NE(Rotation3{}, rotationAroundZ(0.5));
}

// The matrix of a rotation is a linear map, which is printed like its quantities.
TEST(TestActiveRotation, MatrixIsALinearMap) {  // NOLINT
  const Rotation3 rotation = rotationAroundZ(0.5);
  EXPECT_EQ(rotation.getTensor(), Rotation3::TensorT{rotation.getQuantities()});
  EXPECT_EQ(toString(rotation), toString(rotation.getQuantities()));
}

// The tolerance of the validity check is separate from the precision of isApprox(): a rotation whose rounding errors accumulated
// in many compositions is valid, a matrix that deviates by 1e-6 from a rotation is not.
TEST(TestActiveRotation, IsValid) {  // NOLINT
  Rotation3 composition;
  const Rotation3 step = rotationAroundX(0.001) * rotationAroundZ(0.002);
  for (int iteration = 0; iteration < 10000; ++iteration) {
    composition = step * composition;
  }
  EXPECT_TRUE(composition.isValid());
  const auto construct = [&composition] { return Rotation3{composition.getQuantities()}; };
  EXPECT_NO_THROW(construct());
  const Rotation3 deviating = Rotation3::makeUnchecked(1.0 + 1e-6, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
  EXPECT_FALSE(deviating.isValid());
  EXPECT_TRUE(deviating.isValid(1e-5));
  // The determinant of a reflection is -1, whatever the tolerance.
  EXPECT_FALSE(Rotation3::makeUnchecked(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0).isValid(1.0));
  // A matrix with a NaN element is not a rotation.
  const double nan = std::numeric_limits<double>::quiet_NaN();
  EXPECT_FALSE(Rotation3::makeUnchecked(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, nan).isValid(1.0));
}

// The tolerance for float is larger than for double: a float rotation computed with sine and cosine is valid.
TEST(TestActiveRotation, IsValidForFloat) {  // NOLINT
  using RotationF = ActiveSpatialRotation3<ValueMatrix, float, CoordinateSystemA>;
  const float angle = 0.7f;
  const RotationF rotation{std::cos(angle), -std::sin(angle), 0.0f, std::sin(angle), std::cos(angle), 0.0f, 0.0f, 0.0f, 1.0f};
  EXPECT_TRUE((rotation * rotation * rotation).isValid());
  EXPECT_FALSE(RotationF::makeUnchecked(1.001f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f).isValid());
}

// The tolerance for int is 0: an integer rotation, e.g. by pi/2, is valid, an integer shear is not. In debug builds, the
// constructor rejects the shear.
TEST(TestActiveRotation, IsValidForIntegers) {  // NOLINT
  using RotationI = ActiveSpatialRotation3<ValueMatrix, int, CoordinateSystemA>;
  EXPECT_TRUE((RotationI{0, -1, 0, 1, 0, 0, 0, 0, 1}.isValid()));  // Active rotation of pi/2 around z.
  EXPECT_FALSE((RotationI::makeUnchecked(1, 5, 0, 0, 1, 0, 0, 0, 1).isValid()));
  const auto shear = [] { return RotationI{1, 5, 0, 0, 1, 0, 0, 0, 1}; };
  if constexpr (wuzp::internal::checksValidity) {
    EXPECT_THROW(shear(), std::invalid_argument);
  } else {
    EXPECT_NO_THROW(shear());
  }
}

// The determinant of a rotation in two dimensions is ad - bc, in three dimensions the triple product of the columns, and in other
// dimensions that of the value matrix backend.
TEST(TestActiveRotation, IsValidInOtherDimensions) {  // NOLINT
  using Rotation2 = ActiveSpatialRotation2<ValueMatrix, double, CoordinateSystemA>;
  using Rotation4 = ActiveSpatialRotation<ValueMatrix, double, CoordinateSystemA, 4>;
  EXPECT_TRUE((Rotation2{0.0, -1.0, 1.0, 0.0}.isValid()));
  EXPECT_FALSE((Rotation2::makeUnchecked(0.0, 1.0, 1.0, 0.0).isValid()));
  // A rotation by pi/2 in the plane of x and y, and one by pi/2 in the plane of z and w.
  EXPECT_TRUE((Rotation4{0.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 1.0, 0.0}.isValid()));
  // A reflection of w.
  EXPECT_FALSE((Rotation4::makeUnchecked(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0).isValid()));
}

// The projection computes the rotation nearest to a matrix, by the Newton-Schulz iteration: the rotation itself for a rotation, a
// valid rotation close to the original for one whose elements were rounded to three digits, and R for a scaled rotation c R,
// whose polar decomposition is R (c I).
TEST(TestActiveRotation, ProjectToRotation) {  // NOLINT
  // A rotation whose elements are exact is its own projection, without an iteration.
  const Rotation3 exact{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  EXPECT_EQ(Rotation3::projectToRotation(exact.getQuantities()), exact);
  const Rotation3 rotation = rotationAroundZ(0.5) * rotationAroundX(0.3);
  EXPECT_TRUE(Rotation3::projectToRotation(rotation.getQuantities()).isApprox(rotation, 1e-14));
  Rotation3::QuantitiesT rounded = rotation.getQuantities();
  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      rounded.set(row, col, QuantityD<Dimensionless>{std::round(rotation.getQuantities().get(row, col).getValue() * 1000.0) / 1000.0});
    }
  }
  EXPECT_FALSE(Rotation3::makeUnchecked(rounded).isValid());
  const Rotation3 projected = Rotation3::projectToRotation(rounded);
  EXPECT_TRUE(projected.isValid(1e-14));
  EXPECT_TRUE(projected.isApprox(rotation, 1e-3));
  EXPECT_TRUE(Rotation3::projectToRotation(rotation.getQuantities() * 1.2).isApprox(rotation, 1e-14));
}

// The projection needs a matrix close to a rotation, ||I - A^T A|| < 1 in the Frobenius norm, which guarantees that the iteration
// converges, and a positive determinant: it rejects a scaling by 2, a singular matrix, and a reflection. ||I - A^T A|| of a
// scaling c I is sqrt(3) |c^2 - 1|: about 0.76 for c = 1.2, which the projection takes (see ProjectToRotation), and about 1.2
// for c = 1.3, which it rejects. A matrix with a NaN element has the deviation NaN, which it rejects too.
TEST(TestActiveRotation, ProjectToRotationRejectsMatricesFarFromRotations) {  // NOLINT
  EXPECT_THROW(Rotation3::projectToRotation(1.3, 0.0, 0.0, 0.0, 1.3, 0.0, 0.0, 0.0, 1.3), std::invalid_argument);
  const double nan = std::numeric_limits<double>::quiet_NaN();
  EXPECT_THROW(Rotation3::projectToRotation(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, nan), std::invalid_argument);
  EXPECT_THROW(Rotation3::projectToRotation(2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0), std::invalid_argument);
  EXPECT_THROW(Rotation3::projectToRotation(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0), std::invalid_argument);
  EXPECT_THROW(Rotation3::projectToRotation(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0), std::invalid_argument);
}

// A rotation is a linear transformation: it composes with linear transformations into linear transformations, e.g. with a
// scaling, in both orders.
TEST(TestActiveRotation, ComposeWithLinearTransformation) {  // NOLINT
  using Scaling = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 3, 3, Dimensionless>;
  const Scaling scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.0};
  const Rotation3 rotation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};  // Active rotation of pi/2 around x.
  static_assert(std::is_same_v<decltype(scaling * rotation), Scaling>);
  static_assert(std::is_same_v<decltype(rotation * scaling), Scaling>);
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  EXPECT_EQ((scaling * rotation).transform(point), scaling.transform(rotation.transform(point)));
  EXPECT_EQ((rotation * scaling).transform(point), rotation.transform(scaling.transform(point)));
  EXPECT_EQ((scaling * rotation), (Scaling{2.0, 0.0, 0.0, 0.0, 0.0, -4.0, 0.0, 8.0, 0.0}));
  // A rotation conjugates linear transformations: R A R^T.
  EXPECT_TRUE(rotation.transform(scaling).isApprox(Scaling{2.0, 0.0, 0.0, 0.0, 8.0, 0.0, 0.0, 0.0, 4.0}, 1e-15));
}
