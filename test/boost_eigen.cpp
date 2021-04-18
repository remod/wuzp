#include <gtest/gtest.h>

#include "ctm/Frame.hpp"
#include "ctm/Matrix.hpp"
#include "ctm/Transformation.hpp"
#include "ctm/TypeMatrix.hpp"
#include "ctm/boost.hpp"
#include "ctm/eigen.hpp"
#include "ctm/traits.hpp"

using ctm::Frame;
using ctm::TypeMatrix;
using ctm::Matrix;
using ctm::Sum;
using ctm::ProperRigidTransformationEigen;
using boost::units::si::length;
using boost::units::si::area;
using boost::units::si::volume;
using boost::units::si::mass;
using boost::units::si::force;
using boost::units::si::energy;

TEST(BoostEigen, Matrix) { // NOLINT
  struct FrameAName {};
  using FrameA = Frame<FrameAName>;
  using MyValues = Eigen::Matrix<double, 3, 2>;
  using MyFrames = TypeMatrix<3, 2, FrameA, FrameA, FrameA, FrameA, FrameA, FrameA>;
  using MyUnits = TypeMatrix<3, 2, length, area, volume, mass, force, energy>;
  using MyMatrix = Matrix<MyValues, MyFrames, MyUnits>;
  static_assert(std::is_same<Sum<MyMatrix, MyMatrix>::type, MyMatrix>::value);
}

TEST(BoostEigen, ProperRigidTransformation) { // NOLINT
  struct FromFrameName {};
  struct ToFrameName {};
  using FromFrame = Frame<FromFrameName>;
  using ToFrame = Frame<ToFrameName>;
  using VectorValues = Eigen::Vector3d;
  using VectorFrames = TypeMatrix<3, 1, FromFrame>;
  using VectorUnits = TypeMatrix<3, 1, length>;
  using Vector = Matrix<VectorValues, VectorFrames, VectorUnits>;
  const Vector object(VectorValues(1.0, 2.0, 3.0));
  const Eigen::Quaterniond rotation{1.0, 0.0, 0.0, 0.0};
  const Eigen::Vector3d translation(1.0, 2.0, 3.0);
  const ProperRigidTransformationEigen<FromFrame, ToFrame, double> transformation(rotation, translation);
  const auto result = transformation.transform(object);
}
