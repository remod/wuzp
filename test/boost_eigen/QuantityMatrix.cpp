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

class BoostEigenQuantityMatrixF : public ::testing::Test {
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

TEST_F(BoostEigenQuantityMatrixF, Addition) {  // NOLINT
  EXPECT_EQ(getMatrix1() + getMatrix2(), MyMatrix(getValues1() + getValues2()));
}

TEST_F(BoostEigenQuantityMatrixF, Subtraction) {  // NOLINT
  EXPECT_EQ(getMatrix1() - getMatrix2(), MyMatrix(getValues1() - getValues2()));
}

TEST_F(BoostEigenQuantityMatrixF, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getMatrix1(), getMatrix1());
}

TEST_F(BoostEigenQuantityMatrixF, InEqualityOperator) {  // NOLINT
  EXPECT_NE(getMatrix1(), getMatrix1() + getMatrix2());
}

TEST_F(BoostEigenQuantityMatrixF, Block) {  // NOLINT
  using MyValuesB = Eigen::Matrix<int, 2, 2>;
  using MyUnitsB = TypeMatrix<2, 2, area, volume, force, energy>;
  using MyMatrixB = QuantityMatrix<MyValuesB, MyUnitsB>;
  MyValuesB valuesB;
  valuesB << 2, 3, 5, 6;
  MyMatrixB matrixB(valuesB);
  EXPECT_EQ((getMatrix1().block<0, 1, 2, 2>()), matrixB);
}

TEST_F(BoostEigenQuantityMatrixF, Transpose) {  // NOLINT
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

TEST(BoostEigenQuantityMatrix, Product) {  // NOLINT
  using MyValues = Eigen::Matrix<int, 2, 2>;
  using MyUnits = TypeMatrix<2, 2, length>;
  using MyUnitsResult = TypeMatrix<2, 2, area>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;
  using MyMatrixResult = QuantityMatrix<MyValues, MyUnitsResult>;
  MyValues values;
  values << 1, 2, 3, 4;
  MyMatrix matrix;
  matrix.setValues(values);
  EXPECT_EQ(matrix * matrix, MyMatrixResult(values * values));
}

TEST(BoostEigenQuantityMatrix, ScalarProduct) {  // NOLINT
  using MyValues = Eigen::Matrix<int, 4, 1>;
  using MyUnits = TypeMatrix<4, 1, length>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;
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

TEST(BoostEigenQuantityMatrix, Chain) {  // NOLINT
  using MyValues = Eigen::Matrix<int, 2, 2>;
  using MyUnits1 = TypeMatrix<2, 2, length>;
  using MyUnits2 = TypeMatrix<2, 2, area>;
  using MyUnitsResult = TypeMatrix<2, 2, area>;
  using MyMatrix1 = QuantityMatrix<MyValues, MyUnits1>;
  using MyMatrix2 = QuantityMatrix<MyValues, MyUnits2>;
  using MyMatrixResult = QuantityMatrix<MyValues, MyUnitsResult>;
  MyValues values;
  values << 1, 2, 3, 4;
  MyMatrix1 matrix1(values);
  MyMatrix2 matrix2(values);
  EXPECT_EQ(matrix2 + matrix1 * matrix1, MyMatrixResult(values + values * values));
}

TEST_F(BoostEigenQuantityMatrix2, Inverse) {  // NOLINT
  using MyUnitsInv = TypeMatrix<2, 2, frequency>;
  using MyMatrixInv = QuantityMatrix<MyValues, MyUnitsInv>;
  MyValues valuesInv;
  valuesInv << 0.6, -0.7, -0.2, 0.4;
  MyMatrixInv matrixInv(valuesInv);
  EXPECT_TRUE(getMatrix().inverse().isApprox(matrixInv));
}
