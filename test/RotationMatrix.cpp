#include <gtest/gtest.h>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Rotation.hpp"

using namespace wuzp;  // NOLINT

TEST(TestRotationMatrix, TransformVector) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionVector3D<double, FromCoordinateSystem>;
  using Transformation = RotationMatrix3D<double, FromCoordinateSystem, ToCoordinateSystem>;
  using TransformedObject = PositionVector3D<double, ToCoordinateSystem>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector3d(1.0, -2.0, -3.0));
  const Transformation transformation(Transformation::Impl(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix()));
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestRotationMatrix, TransformPosition) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionPoint3D<double, FromCoordinateSystem>;
  using Transformation = RotationMatrix3D<double, FromCoordinateSystem, ToCoordinateSystem>;
  using TransformedObject = PositionPoint3D<double, ToCoordinateSystem>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector3d(1.0, -2.0, -3.0));
  const Transformation transformation(Transformation::Impl(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix()));
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
