#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"
#include "wuzp/trafo/PassiveRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"

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

TEST(TestPassiveProperRigidTransformation2, TransformPosition) {  // NOLINT
  //  const Position2<double, FromCoordinateSystem> object(1.0, 2.0);
  //  const Eigen::Quaterniond rotation{1.0, 0.0, 0.0, 0.0};
  //  const Translation2<double, CoordinateSystemB> translation(1.0, 2.0);
  //  const PassiveProperRigidTransformation2<FromCoordinateSystem, CoordinateSystemB, double> transformation(rotation, translation);
  //  const auto result = transformation.transform(object);
}

TEST(TestPassiveProperRigidTransformation3, TransformVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, double>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 3.0, -2.0);
  const Transformation::Rotation rotation(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation(0.0, 0.0, -1.0);                          // Passive translation of 1 along z.
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveProperRigidTransformation3, TransformPosition) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, double>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 3.0, -3.0);
  const Transformation::Rotation rotation(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation(0.0, 0.0, -1.0);                          // Passive translation of 1 along z.
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveProperRigidTransformation3, TransformVelocityVector) {  // NOLINT
  using Object = VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, double>;
  using TransformedObject = VelocityVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 3.0, -2.0);
  const Transformation::Rotation rotation(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation(0.0, 0.0, -1.0);                          // Passive translation of 1 along z.
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveProperRigidTransformation3, TransformPointCloud) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, TypeMatrix<3, 2, AbsoluteLength>>;
  using Object = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, double>;
  using TransformedObject = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemB>;
  const Object object(1.0, 4.0, 2.0, 5.0, 3.0, 6.0);
  const TransformedObject transformedObject(1.0, 4.0, 3.0, 6.0, -3.0, -6.0);
  const Transformation::Rotation rotation(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation(0.0, 0.0, -1.0);                          // Passive translation of 1 along z.
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveProperRigidTransformation3, TransformPassiveTranslation) {  // NOLINT
  using Object = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD, double>;
  using TransformedObject = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, 3.0, -2.0);
  const Transformation::Rotation rotation(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation(0.0, 0.0, -1.0);                          // Passive translation of 1 along z.
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestPassiveProperRigidTransformation3, TransformPassiveRotation) {  // NOLINT
  using Object = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB>;
  using Transformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemC, CoordinateSystemC, CoordinateSystemD, double>;
  using TransformedObject = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemD, CoordinateSystemA, CoordinateSystemB>;
  const Object object(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  const TransformedObject transformedObject(0.0, 0.0, -1.0, 0.0, 1.0, 0.0, -1.0, 0.0, 0.0);
  const Transformation::Rotation rotation(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0);  // Passive rotation of pi/2 around x.
  const Transformation::Translation translation(0.0, 0.0, -1.0);                          // Passive translation of 1 along z.
  const Transformation transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
