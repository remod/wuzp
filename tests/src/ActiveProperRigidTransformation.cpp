#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"

#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Quantity.hpp"
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Quantity.hpp"
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;
#endif

TEST(TestActiveProperRigidTransformation2, TransformPosition) {  // NOLINT
  //  const Position2<double, FromCoordinateSystem> object(1.0, 2.0);
  //  const Eigen::Quaterniond rotation{1.0, 0.0, 0.0, 0.0};
  //  const Translation2<double, CoordinateSystemB> translation(1.0, 2.0);
  //  const ActiveProperRigidTransformation2<FromCoordinateSystem, CoordinateSystemB, double> transformation(rotation, translation);
  //  const auto result = transformation.transform(object);
}

TEST(TestActiveProperRigidTransformation3, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -3.0, 2.0);
  const Transformation::Rotation rotation(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);  // Active rotation of pi/2 around x.
  const Transformation::Translation translation(0.0, 0.0, 1.0);                           // Active translation of 1 along z.
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveProperRigidTransformation3, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -3.0, 3.0);
  const Transformation::Rotation rotation(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);  // Active rotation of pi/2 around x.
  const Transformation::Translation translation(0.0, 0.0, 1.0);                           // Active translation of 1 along z.
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveProperRigidTransformation3, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -3.0, 2.0);
  const Transformation::Rotation rotation(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);  // Active rotation of pi/2 around x.
  const Transformation::Translation translation(0.0, 0.0, 1.0);                           // Active translation of 1 along z.
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveProperRigidTransformation3, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, TypeMatrix<3, 2, AbsoluteLength>>;
  using Object = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  using Transformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using TransformedObject = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  const Object object(1.0, 4.0, 2.0, 5.0, 3.0, 6.0);
  const TransformedObject transformedObject(1.0, 4.0, -3.0, -6.0, 3.0, 6.0);
  const Transformation::Rotation rotation(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);  // Active rotation of pi/2 around x.
  const Transformation::Translation translation(0.0, 0.0, 1.0);                           // Active translation of 1 along z.
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveProperRigidTransformation3, TransformActiveTranslation) {  // NOLINT
  using Object = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using TransformedObject = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -3.0, 2.0);
  const Transformation::Rotation rotation(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);  // Active rotation of pi/2 around x.
  const Transformation::Translation translation(0.0, 0.0, 1.0);                           // Active translation of 1 along z.
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveProperRigidTransformation3, TransformActiveRotation) {  // NOLINT
  using Object = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
  using TransformedObject = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  const TransformedObject transformedObject(0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0);
  const Transformation::Rotation rotation(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);  // Active rotation of pi/2 around x.
  const Transformation::Translation translation(0.0, 0.0, 1.0);                           // Active translation of 1 along z.
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

// TODO(remod): Add TransformActive/PassiveProperRigidTransformation everywhere
