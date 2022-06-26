#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"

#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;

TEST(TestPassiveTranslation, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 2.0, 3.0);
  const Transformation transformation(0.0, 0.0, -1.0);  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 2.0, 2.0);
  const Transformation transformation(0.0, 0.0, -1.0);  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 2.0, 3.0);
  const Transformation transformation(0.0, 0.0, -1.0);  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, TypeMatrix<3, 2, Length>>;
  using Object = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemB, Absolute>;
  const Object object(1.0, 4.0, 2.0, 5.0, 3.0, 6.0);
  const TransformedObject transformedObject(1.0, 4.0, 2.0, 5.0, 2.0, 5.0);
  const Transformation transformation(0.0, 0.0, -1.0);  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformPassiveTranslation) {  // NOLINT
  using Object = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD>;
  using TransformedObject = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 2.0, 3.0);
  const Transformation transformation(0.0, 0.0, -1.0);  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveTranslation, TransformPassiveRotation) {  // NOLINT
  using Object = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
  using Transformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD>;
  using TransformedObject = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
  const Object object(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  const TransformedObject transformedObject(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  const Transformation transformation(0.0, 0.0, -1.0);  // Passive translation of 1 along z.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

#endif
