#include <gtest/gtest.h>

#include "ctm/core/Frame.hpp"
#include "ctm/core/FramedQuantityMatrix.hpp"
#include "ctm/core/TypeMatrix.hpp"
#include "ctm/core/traits.hpp"
#include "ctm/trafo/Translation.hpp"
#include "ctm/value_matrix/ValueMatrix.hpp"

using ctm::Frame;
using ctm::FramedQuantityMatrix;
using ctm::FrameMatrix;
using ctm::Position2D;
using ctm::Position3D;
using ctm::QuantityMatrix;
using ctm::Translation2D;
using ctm::Translation3D;
using ctm::TypeMatrix;
using ctm::Vector3D;
using namespace ctm::units;         // NOLINT
using namespace ctm::value_matrix;  // NOLINT

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

class TestFramedQuantityMatrix : public ::testing::Test {
 public:
  using MyValues = ValueMatrix<int, 2, 3>;
  using MyUnits = UnitMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyFrames = FrameMatrix<2, 3, FrameA, FrameB, FrameC, FrameD, FrameE, FrameF>;
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

TEST_F(TestFramedQuantityMatrix, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getMatrix(), getMatrix());
}

TEST(TestFramedQuantityMatrix2, ScalarProduct) {  // NOLINT
  using MyValues = ValueMatrix<int, 4, 1>;
  using MyUnits = UnitMatrix<4, 1, Length>;
  using MyFrames = FrameMatrix<4, 1, FrameA, FrameB, FrameC, FrameD>;
  using MyMatrix = FramedQuantityMatrix<MyValues, MyUnits, MyFrames>;
  using MyValuesResult = int;
  using MyUnitsResult = UnitMatrix<1, 1, Area>;
  using MyMatrixResult = QuantityMatrix<MyValuesResult, MyUnitsResult>;
  MyValues values1;
  values1 << 1, 2, 3, 4;
  MyValues values2;
  values2 << 0, 3, -1, 2;
  EXPECT_EQ(MyMatrix(values1).dot(MyMatrix(values2)), MyMatrixResult(11));
  // TODO(remod): How to treat 1x1 matrices? Equal to scalars?
}

TEST_F(TestFramedQuantityMatrix, Transpose) {  // NOLINT
  using MyValuesT = ValueMatrix<int, 3, 2>;
  using MyUnitsT = UnitMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyFramesT = FrameMatrix<3, 2, FrameA, FrameD, FrameB, FrameE, FrameC, FrameF>;
  using MyMatrixT = FramedQuantityMatrix<MyValuesT, MyUnitsT, MyFramesT>;
  MyValuesT valuesT;
  valuesT << 1, 4, 2, 5, 3, 6;
  MyMatrixT matrixT(valuesT);
  EXPECT_EQ(getMatrix().transpose(), matrixT);
}
