#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveRotation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"

#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;
#endif

TEST(TestActiveTranslation, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 2.0, 3.0);
  const Transformation transformation(4.0, 2.0, 1.0);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveTranslation, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(5.0, 4.0, 4.0);
  const Transformation transformation(4.0, 2.0, 1.0);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveTranslation, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using RotationalVelocity = AngularVelocityVector3<ValueMatrix, double, CoordinateSystemA>;  // Against inertial system.
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(-3.0, 13.0, -3.0);
  const Transformation transformation(4.0, 2.0, 1.0);
  const RotationalVelocity rotationalVelocity(1.0, 2.0, 3.0);
  // TODO(remod): Is this correct?
  EXPECT_EQ(object + rotationalVelocity.crossProduct(transformation), transformedObject);
}

TEST(TestActiveTranslation, TransformActiveTranslation) {  // NOLINT
  using Object = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 2.0, 3.0);
  const Transformation transformation(4.0, 2.0, 1.0);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveTranslation, TransformActiveRotation) {  // NOLINT
  using Object = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  const TransformedObject transformedObject(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  const Transformation transformation(4.0, 2.0, 1.0);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
