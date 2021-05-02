#include <gtest/gtest.h>

#include "ctm/Frame.hpp"
#include "ctm/FramedQuantityMatrix.hpp"
#include "ctm/TypeMatrix.hpp"
#include "ctm/boost.hpp"
#include "ctm/boost_eigen.hpp"
#include "ctm/eigen.hpp"
#include "ctm/traits.hpp"

using boost::units::si::area;
using boost::units::si::energy;
using boost::units::si::force;
using boost::units::si::length;
using boost::units::si::mass;
using boost::units::si::volume;
using ctm::Frame;
using ctm::FramedQuantityMatrix;
using ctm::Position2D;
using ctm::Position3D;
using ctm::ProperRigidTransformation2D;
using ctm::ProperRigidTransformation3D;
using ctm::Quaternion;
using ctm::Sum;
using ctm::Translation2D;
using ctm::Translation3D;
using ctm::TypeMatrix;
using ctm::Vector3D;

TEST(BoostEigen_FramedQuantityMatrix, IsSame) {  // NOLINT
  struct FrameAName {};
  using FrameA = Frame<FrameAName>;
  using MyValues = Eigen::Matrix<double, 3, 2>;
  using MyUnits = TypeMatrix<3, 2, length, area, volume, mass, force, energy>;
  using MyFrames = TypeMatrix<3, 2, FrameA, FrameA, FrameA, FrameA, FrameA, FrameA>;
  using MyMatrix = FramedQuantityMatrix<MyValues, MyUnits, MyFrames>;
  static_assert(std::is_same<Sum<MyMatrix, MyMatrix>::type, MyMatrix>::value);
}

TEST(BoostEigen_FramedQuantityMatrix, Transpose) {  // NOLINT
  struct FrameAName {};
  struct FrameBName {};
  struct FrameCName {};
  struct FrameDName {};
  struct FrameEName {};
  struct FrameFName {};
  using FrameA = Frame<FrameAName>;
  using FrameB = Frame<FrameBName>;
  using FrameC = Frame<FrameCName>;
  using FrameD = Frame<FrameDName>;
  using FrameE = Frame<FrameEName>;
  using FrameF = Frame<FrameFName>;
  using MyValues = Eigen::Matrix<int, 2, 3>;
  using MyValuesT = Eigen::Matrix<int, 3, 2>;
  using MyUnits = TypeMatrix<2, 3, length, area, volume, mass, force, energy>;
  using MyUnitsT = TypeMatrix<3, 2, length, mass, area, force, volume, energy>;
  using MyFrames = TypeMatrix<2, 3, FrameA, FrameB, FrameC, FrameD, FrameE, FrameF>;
  using MyFramesT = TypeMatrix<3, 2, FrameA, FrameD, FrameB, FrameE, FrameC, FrameF>;
  using MyMatrix = FramedQuantityMatrix<MyValues, MyUnits, MyFrames>;
  using MyMatrixT = FramedQuantityMatrix<MyValuesT, MyUnitsT, MyFramesT>;
  MyValues values;
  MyValuesT valuesT;
  values << 1, 2, 3, 4, 5, 6;
  valuesT << 1, 4, 2, 5, 3, 6;
  MyMatrix matrix(values);
  MyMatrixT matrixT(valuesT);
  EXPECT_EQ(matrix.transpose(), matrixT);
}

TEST(BoostEigen_ProperRigidTransformation2D, TransformPosition) {  // NOLINT
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

TEST(BoostEigen_ProperRigidTransformation3D, TransformVector) {  // NOLINT
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

TEST(BoostEigen_ProperRigidTransformation3D, TransformPosition) {  // NOLINT
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
