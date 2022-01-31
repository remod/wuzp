#include <gtest/gtest.h>

#include <Eigen/Geometry>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Rotation.hpp"
#include "wuzp/trafo/Translation.hpp"

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

TEST(TestRotation, TransformPositionVector) {  // NOLINT
  struct CoordinateSystemAName {};
  struct CoordinateSystemBName {};
  using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
  using CoordinateSystemB = CoordinateSystem<CoordinateSystemBName>;
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = SpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -2.0, -3.0);
  const Transformation transformation(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix());
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestRotation, TransformPositionPoint) {  // NOLINT
  struct CoordinateSystemAName {};
  struct CoordinateSystemBName {};
  using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
  using CoordinateSystemB = CoordinateSystem<CoordinateSystemBName>;
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = SpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -2.0, -3.0);
  const Transformation transformation(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix());
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestRotation, TransformPointCloud) {  // NOLINT
  struct CoordinateSystemAName {};
  struct CoordinateSystemBName {};
  using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
  using CoordinateSystemB = CoordinateSystem<CoordinateSystemBName>;
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, TypeMatrix<3, 2, AbsoluteLength>>;
  using Object = Tensor<1, 0, CoordinateMatrix<MyQuantityMatrix, TypeMatrix<3, 2, CoordinateSystemA>>>;
  using Transformation = SpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = Tensor<1, 0, CoordinateMatrix<MyQuantityMatrix, TypeMatrix<3, 2, CoordinateSystemB>>>;
  const MyValueMatrix valueMatrixObject(1.0, 4.0, 2.0, 5.0, 3.0, 6.0);
  const Object object(valueMatrixObject);
  const MyValueMatrix valueMatrixTransformedObject(1.0, 4.0, -2.0, -5.0, -3.0, -6.0);
  const TransformedObject transformedObject(valueMatrixTransformedObject);
  // TODO(remod): Get rid of eigen dependency here. Test creating value matrix from implementation.
  const Transformation transformation(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix());
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestRotation, TransformTranslation) {  // NOLINT
  struct CoordinateSystemAName {};
  struct CoordinateSystemBName {};
  struct CoordinateSystemCName {};
  struct CoordinateSystemDName {};
  using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
  using CoordinateSystemB = CoordinateSystem<CoordinateSystemBName>;
  using CoordinateSystemC = CoordinateSystem<CoordinateSystemCName>;
  using CoordinateSystemD = CoordinateSystem<CoordinateSystemDName>;
  using Object = SpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
  using Transformation = SpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD>;
  using TransformedObject = SpatialTranslation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -2.0, -3.0);
  const Transformation transformation(1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, -1.0);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestRotation, TransformRotation) {  // NOLINT
  struct CoordinateSystemAName {};
  struct CoordinateSystemBName {};
  struct CoordinateSystemCName {};
  struct CoordinateSystemDName {};
  using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
  using CoordinateSystemB = CoordinateSystem<CoordinateSystemBName>;
  using CoordinateSystemC = CoordinateSystem<CoordinateSystemCName>;
  using CoordinateSystemD = CoordinateSystem<CoordinateSystemDName>;
  using Object = SpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
  using Transformation = SpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD>;
  using TransformedObject = SpatialRotation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
  const double sqrt2_2 = 0.5 * std::sqrt(2.0);
  const Object object(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  const TransformedObject transformedObject(0.0, sqrt2_2, sqrt2_2, sqrt2_2, 0.5, -0.5, sqrt2_2, -0.5, 0.5);
  const Transformation transformation(1.0, 0.0, 0.0, 0.0, sqrt2_2, -sqrt2_2, 0.0, sqrt2_2, sqrt2_2);
  EXPECT_TRUE(transformation.transform(object).isApprox(transformedObject));
  // TODO(remod): Test converting an object with original and transformed transformation, and check whether they match.
}
