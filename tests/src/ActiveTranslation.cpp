#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveRotation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"

#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

#include "Unit.hpp"

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Value, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Value, Rows, Cols>;

TEST(TestActiveTranslation, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 2.0, 3.0);
  const Transformation transformation(0.0, 0.0, 1.0);  // Active translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveTranslation, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 2.0, 4.0);
  const Transformation transformation(0.0, 0.0, 1.0);  // Active translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveTranslation, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 2.0, 3.0);
  const Transformation transformation(0.0, 0.0, 1.0);  // Active translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveTranslation, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 2, Length>>;
  using Object = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const Object object(1.0, 4.0, 2.0, 5.0, 3.0, 6.0);
  const TransformedObject transformedObject(1.0, 4.0, 2.0, 5.0, 4.0, 7.0);
  const Transformation transformation(0.0, 0.0, 1.0);  // Active translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveTranslation, TransformActiveTranslation) {  // NOLINT
  using Object = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 2.0, 3.0);
  const Transformation transformation(0.0, 0.0, 1.0);  // Active translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveTranslation, TransformActiveRotation) {  // NOLINT
  using Object = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  const TransformedObject transformedObject(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  const Transformation transformation(0.0, 0.0, 1.0);  // Active translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

#endif
