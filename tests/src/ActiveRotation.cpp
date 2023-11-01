#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Value, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Value, Rows, Cols>;

TEST(TestActiveRotation, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -3.0, 2.0);
  const Transformation transformation(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);  // Active rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveRotation, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -3.0, 2.0);
  const Transformation transformation(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);  // Active rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveRotation, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -3.0, 2.0);
  const Transformation transformation(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);  // Active rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveRotation, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 2, Length>>;
  using Object = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  const Object object(1.0, 4.0, 2.0, 5.0, 3.0, 6.0);
  const TransformedObject transformedObject(1.0, 4.0, -3.0, -6.0, 2.0, 5.0);
  const Transformation transformation(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);  // Active rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestActiveRotation, TransformActiveTranslation) {  // NOLINT
  using Object = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using TransformedObject = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -3.0, 2.0);
  const Transformation transformation(1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);  // Active rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

// TEST(TestActiveRotation, TransformActiveRotation) {  // NOLINT
//  using Object = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
//  using Transformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD>;
//  using TransformedObject = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
//  const double sqrt2_2 = 0.5 * std::sqrt(2.0);
//  const Object object(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
//  const TransformedObject transformedObject(0.0, sqrt2_2, sqrt2_2, sqrt2_2, 0.5, -0.5, sqrt2_2, -0.5, 0.5);
//  const Transformation transformation(Eigen::AngleAxisd(M_PI_2, Eigen::Vector3d(1.0, 0.0, 0.0)).toRotationMatrix());
//  EXPECT_TRUE(transformation.transform(object).isApprox(transformedObject));
//  // TODO(remod): Test converting an object with original and transformed transformation, and check whether they match.
//}
