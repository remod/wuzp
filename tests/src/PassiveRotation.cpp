#include <gtest/gtest.h>

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
#include "wuzp_eigen/Values.hpp"
template <typename Scalar, int Rows, int Cols>
using Values = wuzp_eigen::Values<Scalar, Rows, Cols>;

TEST(TestPassiveRotation, TransformPositionVector) {  // NOLINT
  using Object = PositionVector3<Values, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialRotation3<Values, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionVector3<Values, double, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 3.0, -2.0);
  const Transformation transformation(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveRotation, TransformPositionPoint) {  // NOLINT
  using Object = PositionPoint3<Values, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialRotation3<Values, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionPoint3<Values, double, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 3.0, -2.0);
  const Transformation transformation(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveRotation, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<Values, double, CoordinateSystemA>;
  using Transformation = PassiveSpatialRotation3<Values, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = VelocityVector3<Values, double, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 3.0, -2.0);
  const Transformation transformation(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveRotation, TransformPointCloud) {  // NOLINT
  using MyValues = Values<double, 3, 2>;
  using MyQuantities = Quantities<MyValues, TypeMatrix<3, 2, Length>>;
  using Object = Tensor<1, 0, MyQuantities, CoordinateSystemA, Absolute>;
  using Transformation = PassiveSpatialRotation3<Values, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = Tensor<1, 0, MyQuantities, CoordinateSystemB, Absolute>;
  const Object object(1.0, 4.0, 2.0, 5.0, 3.0, 6.0);
  const TransformedObject transformedObject(1.0, 4.0, 3.0, 6.0, -2.0, -5.0);
  const Transformation transformation(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveRotation, TransformPassiveTranslation) {  // NOLINT
  using Object = PassiveSpatialTranslation3<Values, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
  using Transformation = PassiveSpatialRotation3<Values, double, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD>;
  using TransformedObject = PassiveSpatialTranslation3<Values, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 3.0, -2.0);
  const Transformation transformation(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);  // Passive rotation of pi/2 around x.
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

// TEST(TestPassiveRotation, TransformPassiveRotation) {  // NOLINT
//  using Object = PassiveSpatialRotation3<Values, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
//  using Transformation = PassiveSpatialRotation3<Values, double, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD>;
//  using TransformedObject = PassiveSpatialRotation3<Values, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
//  const double sqrt2_2 = 0.5 * std::sqrt(2.0);
//  const Object object(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
//  const TransformedObject transformedObject(0.0, sqrt2_2, sqrt2_2, sqrt2_2, 0.5, -0.5, sqrt2_2, -0.5, 0.5);
//  const Transformation transformation(Eigen::AngleAxisd(M_PI_2, Eigen::Vector3d(1.0, 0.0, 0.0)).toRotationMatrix());
//  EXPECT_TRUE(transformation.transform(object).isApprox(transformedObject));
//  // TODO(remod): Test converting an object with original and transformed transformation, and check whether they match.
//}

#endif
