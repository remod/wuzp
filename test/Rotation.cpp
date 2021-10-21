#include <gtest/gtest.h>

#include <Eigen/Geometry>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Rotation.hpp"

using namespace wuzp;  // NOLINT

TEST(TestRotation, TransformPositionVector) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionVector3D<double, FromCoordinateSystem>;
  using Transformation = Rotation3D<double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem>;
  using TransformedObject = PositionVector3D<double, ToCoordinateSystem>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector3d(1.0, -2.0, -3.0));
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
  using Object = PositionPoint3D<double, FromCoordinateSystem>;
  using Transformation = Rotation3D<double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem>;
  using TransformedObject = PositionPoint3D<double, ToCoordinateSystem>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector3d(1.0, -2.0, -3.0));
  const Transformation transformation(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix());
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestRotation, TransformPointCloud) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using ValueMatrix = Eigen::Matrix<double, 3, 2>;
  using Object =
      Tensor<1, 0, CoordinateMatrix<ValueMatrix, UnitMatrix<3, 2, Absolute<Length>>, CoordinateSystemMatrix<3, 2, FromCoordinateSystem>>>;
  using Transformation = Rotation3D<double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem>;
  using TransformedObject =
      Tensor<1, 0, CoordinateMatrix<ValueMatrix, UnitMatrix<3, 2, Absolute<Length>>, CoordinateSystemMatrix<3, 2, ToCoordinateSystem>>>;
  ValueMatrix valueMatrixObject;
  valueMatrixObject << 1.0, 4.0, 2.0, 5.0, 3.0, 6.0;
  const Object object(valueMatrixObject);
  ValueMatrix valueMatrixTransformedObject;
  valueMatrixTransformedObject << 1.0, 4.0, -2.0, -5.0, -3.0, -6.0;
  const TransformedObject transformedObject(valueMatrixTransformedObject);
  const Transformation transformation(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix());
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
