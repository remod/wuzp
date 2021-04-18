#include <gtest/gtest.h>

#include "ctm/Frame.hpp"
#include "ctm/Matrix.hpp"
#include "ctm/Transformation.hpp"
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
using ctm::Matrix;
using ctm::Position;
using ctm::ProperRigidTransformation;
using ctm::Sum;
using ctm::Translation;
using ctm::TypeMatrix;

TEST(BoostEigen, Matrix) {  // NOLINT
  struct FrameAName {};
  using FrameA = Frame<FrameAName>;
  using MyValues = Eigen::Matrix<double, 3, 2>;
  using MyFrames = TypeMatrix<3, 2, FrameA, FrameA, FrameA, FrameA, FrameA, FrameA>;
  using MyUnits = TypeMatrix<3, 2, length, area, volume, mass, force, energy>;
  using MyMatrix = Matrix<MyValues, MyFrames, MyUnits>;
  static_assert(std::is_same<Sum<MyMatrix, MyMatrix>::type, MyMatrix>::value);
}

TEST(BoostEigen, ProperRigidTransformation) {  // NOLINT
  struct FromFrameName {};
  struct ToFrameName {};
  using FromFrame = Frame<FromFrameName>;
  using ToFrame = Frame<ToFrameName>;
  const Position<double, FromFrame> object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const Eigen::Quaterniond rotation{1.0, 0.0, 0.0, 0.0};
  const Translation<double, ToFrame> translation(Eigen::Vector3d(1.0, 2.0, 3.0));
  const ProperRigidTransformation<FromFrame, ToFrame, double> transformation(rotation, translation);
  ctm::TransformationTraits<ProperRigidTransformation<FromFrame, ToFrame, double>, Position<double, FromFrame>>::transform(transformation,
                                                                                                                           object);
  const auto result = transformation.transform(object);
}
