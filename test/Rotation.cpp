#include <gtest/gtest.h>

#include <Eigen/Geometry>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Rotation.hpp"

using namespace wuzp;  // NOLINT

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp/units/wuzp/Quantity.hpp"
#include "wuzp/units/wuzp/Unit.hpp"
template <typename Scalar, int Rows, int Cols>
using Quantity = wuzp::Quantity<Scalar, Rows, Cols>;
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

TEST(TestRotation, TransformPositionVector) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionVector3D<ValueMatrix, double, FromCoordinateSystem>;
  using Transformation = Rotation3D<ValueMatrix, double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem>;
  using TransformedObject = PositionVector3D<ValueMatrix, double, ToCoordinateSystem>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -2.0, -3.0);
  const Transformation transformation(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix());
  auto result = TransformedObject::zero();
  result = transformation.transform(object);
  EXPECT_EQ(result, transformedObject);
}

TEST(TestRotation, TransformPositionPoint) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionPoint3D<ValueMatrix, double, FromCoordinateSystem>;
  using Transformation = Rotation3D<ValueMatrix, double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem>;
  using TransformedObject = PositionPoint3D<ValueMatrix, double, ToCoordinateSystem>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(1.0, -2.0, -3.0);
  const Transformation transformation(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix());
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestRotation, TransformPointCloud) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, TypeMatrix<3, 2, AbsoluteLength>>;
  using Object = Tensor<1, 0, CoordinateMatrix<MyQuantityMatrix, TypeMatrix<3, 2, FromCoordinateSystem>>>;
  using Transformation = Rotation3D<ValueMatrix, double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem>;
  using TransformedObject = Tensor<1, 0, CoordinateMatrix<MyQuantityMatrix, TypeMatrix<3, 2, ToCoordinateSystem>>>;
  const MyValueMatrix valueMatrixObject(1.0, 4.0, 2.0, 5.0, 3.0, 6.0);
  const Object object(valueMatrixObject);
  const MyValueMatrix valueMatrixTransformedObject(1.0, 4.0, -2.0, -5.0, -3.0, -6.0);
  const TransformedObject transformedObject(valueMatrixTransformedObject);
  const Transformation transformation(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix());
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
