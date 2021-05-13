#include <gtest/gtest.h>

#include "ctm/boost/boost.hpp"
#include "ctm/boost_eigen/boost_eigen.hpp"
#include "ctm/core/Frame.hpp"
#include "ctm/core/FramedQuantityMatrix.hpp"
#include "ctm/core/TypeMatrix.hpp"
#include "ctm/core/traits.hpp"
#include "ctm/eigen/eigen.hpp"

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
using ctm::QuantityMatrix;
using ctm::Translation2D;
using ctm::Translation3D;
using ctm::TypeMatrix;
using ctm::Vector3D;

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

class BoostEigenFramedQuantityMatrix : public ::testing::Test {
 public:
  using MyValues = Eigen::Matrix<int, 2, 3>;
  using MyUnits = TypeMatrix<2, 3, length, area, volume, mass, force, energy>;
  using MyFrames = TypeMatrix<2, 3, FrameA, FrameB, FrameC, FrameD, FrameE, FrameF>;
  using MyMatrix = FramedQuantityMatrix<MyValues, MyUnits, MyFrames>;

  const MyMatrix& getMatrix() const { return matrix_; }

 protected:
  void SetUp() override {
    values_ << 1, 2, 3, 4, 5, 6;
    matrix_.setValues(values_);
  }

 private:
  MyValues values_;
  MyMatrix matrix_;
};

TEST_F(BoostEigenFramedQuantityMatrix, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getMatrix(), getMatrix());
}

TEST(BoostEigenFramedQuantityMatrix2, ScalarProduct) {  // NOLINT
  using MyValues = Eigen::Matrix<int, 4, 1>;
  using MyUnits = TypeMatrix<4, 1, length>;
  using MyFrames = TypeMatrix<4, 1, FrameA, FrameB, FrameC, FrameD>;
  using MyMatrix = FramedQuantityMatrix<MyValues, MyUnits, MyFrames>;
  using MyValuesResult = int;
  using MyUnitsResult = TypeMatrix<1, 1, area>;
  using MyMatrixResult = QuantityMatrix<MyValuesResult, MyUnitsResult>;
  MyValues values1;
  values1 << 1, 2, 3, 4;
  MyValues values2;
  values2 << 0, 3, -1, 2;
  EXPECT_EQ(MyMatrix(values1).dot(MyMatrix(values2)), MyMatrixResult(11));
  // TODO(remod): How to treat 1x1 matrices? Equal to scalars?
}

TEST_F(BoostEigenFramedQuantityMatrix, Transpose) {  // NOLINT
  using MyValuesT = Eigen::Matrix<int, 3, 2>;
  using MyUnitsT = TypeMatrix<3, 2, length, mass, area, force, volume, energy>;
  using MyFramesT = TypeMatrix<3, 2, FrameA, FrameD, FrameB, FrameE, FrameC, FrameF>;
  using MyMatrixT = FramedQuantityMatrix<MyValuesT, MyUnitsT, MyFramesT>;
  MyValuesT valuesT;
  valuesT << 1, 4, 2, 5, 3, 6;
  MyMatrixT matrixT(valuesT);
  EXPECT_EQ(getMatrix().transpose(), matrixT);
}
