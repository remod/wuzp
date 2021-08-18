#include <gtest/gtest.h>

#include "ctm/core/Frame.hpp"
#include "ctm/core/FramedQuantityMatrix.hpp"
#include "ctm/core/traits.hpp"
#include "ctm/trafo/Rotation.hpp"

using ctm::Frame;
using ctm::Position2D;
using ctm::Position3D;
using ctm::RotationMatrix2D;
using ctm::RotationMatrix3D;
using ctm::Vector3D;

TEST(TestRotationMatrix, TransformVector) {  // NOLINT
  struct FromFrameName {};
  struct ToFrameName {};
  using FromFrame = Frame<FromFrameName>;
  using ToFrame = Frame<ToFrameName>;
  using Object = Vector3D<double, FromFrame>;
  using Transformation = RotationMatrix3D<double, FromFrame, ToFrame>;
  using TransformedObject = Vector3D<double, ToFrame>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector3d(1.0, -2.0, -3.0));
  const Transformation transformation(Transformation::Impl(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix()));
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestRotationMatrix, TransformPosition) {  // NOLINT
  struct FromFrameName {};
  struct ToFrameName {};
  using FromFrame = Frame<FromFrameName>;
  using ToFrame = Frame<ToFrameName>;
  using Object = Position3D<double, FromFrame>;
  using Transformation = RotationMatrix3D<double, FromFrame, ToFrame>;
  using TransformedObject = Position3D<double, ToFrame>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector3d(1.0, -2.0, -3.0));
  const Transformation transformation(Transformation::Impl(Eigen::Quaternion<double>(0.0, 1.0, 0.0, 0.0).toRotationMatrix()));
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
