#include <gtest/gtest.h>

#include "ctm/boost/boost.hpp"
#include "ctm/core/QuantityMatrix.hpp"
#include "ctm/core/TypeMatrix.hpp"
#include "ctm/core/traits.hpp"
#include "ctm/eigen/eigen.hpp"

using boost::units::si::area;
using boost::units::si::energy;
using boost::units::si::force;
using boost::units::si::frequency;
using boost::units::si::length;
using boost::units::si::mass;
using boost::units::si::volume;
using ctm::QuantityMatrix;
using ctm::TypeMatrix;

class BoostEigenQuantityMatrix : public ::testing::Test {
 public:
  using MyValues = Eigen::Matrix<int, 2, 3>;
  using MyUnits = TypeMatrix<2, 3, length, area, volume, mass, force, energy>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;

  const MyValues& getValues1() const { return values1_; }
  const MyValues& getValues2() const { return values2_; }
  const MyMatrix& getMatrix1() const { return matrix1_; }
  const MyMatrix& getMatrix2() const { return matrix2_; }

 protected:
  void SetUp() override {
    values1_ << 1, 2, 3, 4, 5, 6;
    matrix1_.setValues(values1_);
    values2_ << 3, 1, 4, 1, 5, 9;
    matrix2_.setValues(values2_);
  }

 private:
  MyValues values1_;
  MyMatrix matrix1_;
  MyValues values2_;
  MyMatrix matrix2_;
};

TEST_F(BoostEigenQuantityMatrix, Addition) {  // NOLINT
  EXPECT_EQ(getMatrix1() + getMatrix2(), MyMatrix(getValues1() + getValues2()));
}

TEST_F(BoostEigenQuantityMatrix, Subtraction) {  // NOLINT
  EXPECT_EQ(getMatrix1() - getMatrix2(), MyMatrix(getValues1() - getValues2()));
}

TEST_F(BoostEigenQuantityMatrix, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getMatrix1(), getMatrix1());
}

TEST_F(BoostEigenQuantityMatrix, InEqualityOperator) {  // NOLINT
  EXPECT_NE(getMatrix1(), getMatrix1() + getMatrix2());
}

TEST_F(BoostEigenQuantityMatrix, Block) {  // NOLINT
  using MyValuesB = Eigen::Matrix<int, 2, 2>;
  using MyUnitsB = TypeMatrix<2, 2, area, volume, force, energy>;
  using MyMatrixB = QuantityMatrix<MyValuesB, MyUnitsB>;
  MyValuesB valuesB;
  valuesB << 2, 3, 5, 6;
  MyMatrixB matrixB(valuesB);
  EXPECT_EQ((getMatrix1().block<0, 1, 2, 2>()), matrixB);
}

TEST_F(BoostEigenQuantityMatrix, Transpose) {  // NOLINT
  using MyValuesT = Eigen::Matrix<int, 3, 2>;
  using MyUnitsT = TypeMatrix<3, 2, length, mass, area, force, volume, energy>;
  using MyMatrixT = QuantityMatrix<MyValuesT, MyUnitsT>;
  MyValuesT valuesT;
  valuesT << 1, 4, 2, 5, 3, 6;
  MyMatrixT matrixT(valuesT);
  EXPECT_EQ(getMatrix1().transpose(), matrixT);
}

class BoostEigenQuantityMatrix2 : public ::testing::Test {
 public:
  using MyValues = Eigen::Matrix<double, 2, 2>;
  using MyUnits = TypeMatrix<2, 2, boost::units::si::time>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;

  const MyValues& getValues() const { return values_; }
  const MyMatrix& getMatrix() const { return matrix_; }

 protected:
  void SetUp() override {
    values_ << 4.0, 7.0, 2.0, 6.0;
    matrix_.setValues(values_);
  }

 private:
  MyValues values_;
  MyMatrix matrix_;
};

TEST_F(BoostEigenQuantityMatrix2, Multiplication) {  // NOLINT
  EXPECT_EQ(getMatrix() * getMatrix(), MyMatrix(getValues() * getValues()));
}

TEST_F(BoostEigenQuantityMatrix2, Chain) {  // NOLINT
                                            // TODO(remod): Support all eigen objects, not just matrices.
  //  EXPECT_EQ(getMatrix() + getMatrix() * getMatrix(), MyMatrix(getValues() + getValues() * getValues()));
}

TEST_F(BoostEigenQuantityMatrix2, Inverse) {  // NOLINT
  using MyUnitsInv = TypeMatrix<2, 2, frequency>;
  using MyMatrixInv = QuantityMatrix<MyValues, MyUnitsInv>;
  MyValues valuesInv;
  valuesInv << 0.6, -0.7, -0.2, 0.4;
  MyMatrixInv matrixInv(valuesInv);
  EXPECT_TRUE(getMatrix().inverse().isApprox(matrixInv));
}
