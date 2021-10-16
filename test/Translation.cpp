#include <gtest/gtest.h>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Translation.hpp"

using namespace wuzp;  // NOLINT

TEST(TestTranslation, TransformPositionVector) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionVector3D<double, FromCoordinateSystem>;
  using Transformation = SpatialTranslation3D<double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem>;
  using TransformedObject = PositionVector3D<double, ToCoordinateSystem>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector3d(1.0, 2.0, 3.0));
  const Transformation transformation(Eigen::Vector3d(4.0, 2.0, 1.0));
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestTranslation, TransformPositionPoint) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionPoint3D<double, FromCoordinateSystem>;
  using Transformation = SpatialTranslation3D<double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem>;
  using TransformedObject = PositionPoint3D<double, ToCoordinateSystem>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector3d(5.0, 4.0, 4.0));
  const Transformation transformation(Eigen::Vector3d(4.0, 2.0, 1.0));
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
