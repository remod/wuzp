#include <gtest/gtest.h>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ProperRigidTransformation.hpp"

using namespace wuzp;  // NOLINT

TEST(TestProperRigidTransformation2D, TransformPosition) {  // NOLINT
  //  struct FromCoordinateSystemName {};
  //  struct ToCoordinateSystemName {};
  //  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  //  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  //  const Position2D<double, FromCoordinateSystem> object(Eigen::Vector2d(1.0, 2.0));
  //  const Eigen::Quaterniond rotation{1.0, 0.0, 0.0, 0.0};
  //  const Translation2D<double, ToCoordinateSystem> translation(Eigen::Vector2d(1.0, 2.0));
  //  const ProperRigidTransformation2D<FromCoordinateSystem, ToCoordinateSystem, double> transformation(rotation, translation);
  //  const auto result = transformation.transform(object);
}

TEST(TestProperRigidTransformation3D, TransformVector) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct IntermediateCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using IntermediateCoordinateSystem = CoordinateSystem<IntermediateCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionVector3D<double, FromCoordinateSystem>;
  using TransformedObject = PositionVector3D<double, ToCoordinateSystem>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector3d(1.0, -2.0, -3.0));
  const Quaternion<double, FromCoordinateSystem, IntermediateCoordinateSystem> rotation{0.0, 1.0, 0.0, 0.0};
  const Translation3D<double, IntermediateCoordinateSystem, ToCoordinateSystem> translation(
      PositionVector3D<double, IntermediateCoordinateSystem>(Eigen::Vector3d(4.0, 2.0, 1.0)));
  const ProperRigidTransformation3D<FromCoordinateSystem, IntermediateCoordinateSystem, ToCoordinateSystem, double> transformation(
      rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestProperRigidTransformation3D, TransformPosition) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct IntermediateCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using IntermediateCoordinateSystem = CoordinateSystem<IntermediateCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  using Object = PositionPoint3D<double, FromCoordinateSystem>;
  using TransformedObject = PositionPoint3D<double, ToCoordinateSystem>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector3d(5.0, 0.0, -2.0));
  const Quaternion<double, FromCoordinateSystem, IntermediateCoordinateSystem> rotation{0.0, 1.0, 0.0, 0.0};
  const Translation3D<double, IntermediateCoordinateSystem, ToCoordinateSystem> translation(
      PositionVector3D<double, IntermediateCoordinateSystem>(Eigen::Vector3d(4.0, 2.0, 1.0)));
  const ProperRigidTransformation3D<FromCoordinateSystem, IntermediateCoordinateSystem, ToCoordinateSystem, double> transformation(
      rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
