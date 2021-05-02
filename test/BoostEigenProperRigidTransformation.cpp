#include <gtest/gtest.h>

#include "ctm/Frame.hpp"
#include "ctm/boost_eigen.hpp"
#include "ctm/eigen.hpp"
#include "ctm/traits.hpp"

using ctm::Frame;
using ctm::Position2D;
using ctm::Position3D;
using ctm::ProperRigidTransformation2D;
using ctm::ProperRigidTransformation3D;
using ctm::Quaternion;
using ctm::Translation2D;
using ctm::Translation3D;
using ctm::Vector3D;

TEST(BoostEigenProperRigidTransformation2D, TransformPosition) {  // NOLINT
  //  struct FromFrameName {};
  //  struct ToFrameName {};
  //  using FromFrame = Frame<FromFrameName>;
  //  using ToFrame = Frame<ToFrameName>;
  //  const Position2D<double, FromFrame> object(Eigen::Vector2d(1.0, 2.0));
  //  const Eigen::Quaterniond rotation{1.0, 0.0, 0.0, 0.0};
  //  const Translation2D<double, ToFrame> translation(Eigen::Vector2d(1.0, 2.0));
  //  const ProperRigidTransformation2D<FromFrame, ToFrame, double> transformation(rotation, translation);
  //  const auto result = transformation.transform(object);
}

TEST(BoostEigenProperRigidTransformation3D, TransformVector) {  // NOLINT
  struct FromFrameName {};
  struct IntermediateFrameName {};
  struct ToFrameName {};
  using FromFrame = Frame<FromFrameName>;
  using IntermediateFrame = Frame<IntermediateFrameName>;
  using ToFrame = Frame<ToFrameName>;
  using Object = Vector3D<double, FromFrame>;
  using TransformedObject = Vector3D<double, ToFrame>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector3d(1.0, -2.0, -3.0));
  const Quaternion<double, FromFrame, IntermediateFrame> rotation{0.0, 1.0, 0.0, 0.0};
  const Translation3D<double, IntermediateFrame, ToFrame> translation(Vector3D<double, IntermediateFrame>(Eigen::Vector3d(4.0, 2.0, 1.0)));
  const ProperRigidTransformation3D<FromFrame, IntermediateFrame, ToFrame, double> transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(BoostEigenProperRigidTransformation3D, TransformPosition) {  // NOLINT
  struct FromFrameName {};
  struct IntermediateFrameName {};
  struct ToFrameName {};
  using FromFrame = Frame<FromFrameName>;
  using IntermediateFrame = Frame<IntermediateFrameName>;
  using ToFrame = Frame<ToFrameName>;
  using Object = Position3D<double, FromFrame>;
  using TransformedObject = Position3D<double, ToFrame>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector3d(5.0, 0.0, -2.0));
  const Quaternion<double, FromFrame, IntermediateFrame> rotation{0.0, 1.0, 0.0, 0.0};
  const Translation3D<double, IntermediateFrame, ToFrame> translation(Vector3D<double, IntermediateFrame>(Eigen::Vector3d(4.0, 2.0, 1.0)));
  const ProperRigidTransformation3D<FromFrame, IntermediateFrame, ToFrame, double> transformation(rotation, translation);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
