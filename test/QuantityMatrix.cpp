#include <gtest/gtest.h>

#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/units/Unit.hpp"
#include "wuzp/units/UnitMatrix.hpp"
#include "wuzp/value_matrix/ValueMatrix.hpp"

#include "test_compilation_error.hpp"

using namespace wuzp;  // NOLINT

TEST(TestQuantityMatrix2, ConstructFromGoodQuantities) {  // NOLINT
  using MyValues = ValueMatrix<int, 4, 1>;
  using MyUnits = UnitMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;
  MyMatrix myMatrix(QuantityTraits::Create<Quantity<int, Length>>::create(1), QuantityTraits::Create<Quantity<int, Area>>::create(2),
                    QuantityTraits::Create<Quantity<int, Volume>>::create(3), QuantityTraits::Create<Quantity<int, Mass>>::create(4));
}

TEST(TestQuantityMatrix2, ConstructFromBadQuantities) {  // NOLINT
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/units/Quantity.hpp"
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/units/Unit.hpp"
#include "wuzp/units/UnitMatrix.hpp"
#include "wuzp/value_matrix/ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

int main() {
  using MyValues = ValueMatrix<int, 4, 1>;
  using MyUnits = UnitMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;
  MyMatrix myMatrix(QuantityTraits::Create<Quantity<int, Length>>::create(1),
                    QuantityTraits::Create<Quantity<int, Area>>::create(2),
                    QuantityTraits::Create<Quantity<int, Volume>>::create(3),
                    QuantityTraits::Create<Quantity<int, Length>>::create(4));
  return 0;
}
  )",
                                               "Unit type mismatch.")
}

class TestQuantityMatrix : public ::testing::Test {
 public:
  using MyValues = ValueMatrix<int, 2, 3>;
  using MyUnits = UnitMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;

  const MyValues& getValues1() const { return values1_; }
  const MyValues& getValues2() const { return values2_; }
  const MyMatrix& getMatrix1() const { return matrix1_; }
  const MyMatrix& getMatrix2() const { return matrix2_; }
  MyMatrix& getMatrix1() { return matrix1_; }

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

TEST_F(TestQuantityMatrix, SetGetValues) {  // NOLINT
  EXPECT_EQ(getMatrix1().getValues(), getValues1());
  getMatrix1().setValues(getValues2());
  EXPECT_EQ(getMatrix1().getValues(), getValues2());
}

TEST_F(TestQuantityMatrix, SetGetValue) {  // NOLINT
  EXPECT_EQ((getMatrix1().getValue<1, 2>()), getValues1()(1, 2));
  const int value{7};
  getMatrix1().setValue<1, 2>(value);
  EXPECT_EQ((getMatrix1().getValue<1, 2>()), value);
}

TEST_F(TestQuantityMatrix, SetGetQuantity) {  // NOLINT
  using EnergyQuantity = Quantity<int, Energy>;
  EXPECT_EQ((getMatrix1().getQuantity<1, 2>()), QuantityTraits::Create<EnergyQuantity>::create(getValues1()(1, 2)));
  const auto quantity = QuantityTraits::Create<EnergyQuantity>::create(7);
  getMatrix1().setQuantity<1, 2>(quantity);
  EXPECT_EQ((getMatrix1().getQuantity<1, 2>()), quantity);
}

TEST(TestQuantityMatrixStandalone, SetBadQuantity){// NOLINT
                                                   ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/units/Quantity.hpp"
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/units/Unit.hpp"
#include "wuzp/units/UnitMatrix.hpp"
#include "wuzp/value_matrix/ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

int main() {
  using MyValues = ValueMatrix<int, 2, 3>;
  using MyUnits = UnitMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;
  MyMatrix matrix;
  matrix.setQuantity<1, 2>(QuantityTraits::Create<Quantity<int, Time>>::create(7));
  return 0;
}
  )",
                                                                                                "Unit type mismatch.")}

TEST_F(TestQuantityMatrix, Addition) {  // NOLINT
  EXPECT_EQ(getMatrix1() + getMatrix2(), MyMatrix(getValues1() + getValues2()));
}

TEST_F(TestQuantityMatrix, Subtraction) {  // NOLINT
  EXPECT_EQ(getMatrix1() - getMatrix2(), MyMatrix(getValues1() - getValues2()));
}

TEST_F(TestQuantityMatrix, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getMatrix1(), getMatrix1());
}

TEST_F(TestQuantityMatrix, InequalityOperator) {  // NOLINT
  EXPECT_NE(getMatrix1(), getMatrix1() + getMatrix2());
}

TEST_F(TestQuantityMatrix, Block) {  // NOLINT
  using MyValuesB = ValueMatrix<int, 2, 2>;
  using MyUnitsB = UnitMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyMatrixB = QuantityMatrix<MyValuesB, MyUnitsB>;
  MyValuesB valuesB;
  valuesB << 2, 3, 5, 6;
  MyMatrixB matrixB(valuesB);
  EXPECT_EQ((getMatrix1().block<0, 1, 2, 2>()), matrixB);
}

TEST_F(TestQuantityMatrix, Transpose) {  // NOLINT
  using MyValuesT = ValueMatrix<int, 3, 2>;
  using MyUnitsT = UnitMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyMatrixT = QuantityMatrix<MyValuesT, MyUnitsT>;
  MyValuesT valuesT;
  valuesT << 1, 4, 2, 5, 3, 6;
  MyMatrixT matrixT(valuesT);
  EXPECT_EQ(getMatrix1().transpose(), matrixT);
}

TEST(TestQuantityMatrix2, ProductQuantityMatrix) {  // NOLINT
  using MyValues = ValueMatrix<int, 2, 2>;
  using MyUnits = UnitMatrix<2, 2, Length>;
  using MyUnitsResult = UnitMatrix<2, 2, Area>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;
  using MyMatrixResult = QuantityMatrix<MyValues, MyUnitsResult>;
  MyValues values;
  values << 1, 2, 3, 4;
  MyMatrix matrix;
  matrix.setValues(values);
  EXPECT_EQ(matrix * matrix, MyMatrixResult(values * values));
}

TEST(TestQuantityMatrix2, ProductQuantity) {  // NOLINT
  using MyValues = ValueMatrix<int, 2, 2>;
  using MyUnits = UnitMatrix<2, 2, Length>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;
  using MyValue = int;
  using MyQuantity = Quantity<MyValue, Length>;
  using MyUnitsResult = UnitMatrix<2, 2, Area>;
  using MyMatrixResult = QuantityMatrix<MyValues, MyUnitsResult>;
  const MyValue value{2};
  const auto quantity = QuantityTraits::Create<MyQuantity>::create(value);
  MyValues values;
  values << 1, 2, 3, 4;
  MyMatrix matrix;
  matrix.setValues(values);
  EXPECT_EQ(matrix * quantity, MyMatrixResult(values * value));
  EXPECT_EQ(quantity * matrix, MyMatrixResult(values * value));
}

TEST(TestQuantityMatrix2, DivisionQuantity) {  // NOLINT
  using MyValues = ValueMatrix<int, 2, 2>;
  using MyUnits = UnitMatrix<2, 2, Length>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;
  using MyValue = int;
  using MyQuantity = Quantity<MyValue, Length>;
  using MyUnitsResult = UnitMatrix<2, 2, Dimensionless>;
  using MyMatrixResult = QuantityMatrix<MyValues, MyUnitsResult>;
  const MyValue value{2};
  const auto quantity = QuantityTraits::Create<MyQuantity>::create(value);
  MyValues values;
  values << 1, 2, 3, 4;
  MyMatrix matrix;
  matrix.setValues(values);
  EXPECT_EQ(matrix / quantity, MyMatrixResult(values / value));
}

TEST(TestQuantityMatrix2, ScalarProduct) {  // NOLINT
  using MyValues = ValueMatrix<int, 4, 1>;
  using MyUnits = UnitMatrix<4, 1, Length>;
  using MyMatrix = QuantityMatrix<MyValues, MyUnits>;
  using MyMatrixResult = Quantity<int, Area>;
  MyValues values1;
  values1 << 1, 2, 3, 4;
  MyValues values2;
  values2 << 0, 3, -1, 2;
  EXPECT_EQ(MyMatrix(values1).dot(MyMatrix(values2)), QuantityTraits::Create<MyMatrixResult>::create(11));
}

TEST(TestQuantityMatrix2, Chain) {  // NOLINT
  using MyValues = ValueMatrix<int, 2, 2>;
  using MyUnits1 = UnitMatrix<2, 2, Length>;
  using MyUnits2 = UnitMatrix<2, 2, Area>;
  using MyUnitsResult = UnitMatrix<2, 2, Area>;
  using MyMatrix1 = QuantityMatrix<MyValues, MyUnits1>;
  using MyMatrix2 = QuantityMatrix<MyValues, MyUnits2>;
  using MyMatrixResult = QuantityMatrix<MyValues, MyUnitsResult>;
  MyValues values;
  values << 1, 2, 3, 4;
  MyMatrix1 matrix1(values);
  MyMatrix2 matrix2(values);
  EXPECT_EQ(matrix2 + matrix1 * matrix1, MyMatrixResult(values + values * values));
}

class TestQuantityMatrixInvertible : public ::testing::Test {
 public:
  using MyValues = ValueMatrix<double, 2, 2>;
  using MyUnits = UnitMatrix<2, 2, Time>;
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

TEST_F(TestQuantityMatrixInvertible, Inverse) {  // NOLINT
  using MyUnitsInv = UnitMatrix<2, 2, Frequency>;
  using MyMatrixInv = QuantityMatrix<MyValues, MyUnitsInv>;
  MyValues valuesInv;
  valuesInv << 0.6, -0.7, -0.2, 0.4;
  MyMatrixInv matrixInv(valuesInv);
  EXPECT_TRUE(getMatrix().inverse().isApprox(matrixInv));
}
