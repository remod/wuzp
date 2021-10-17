#include <gtest/gtest.h>

#include "wuzp/core/QuantityMatrix.hpp"
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/units/Unit.hpp"
#include "wuzp/units/UnitMatrix.hpp"
#include "wuzp/value_matrix/ValueMatrix.hpp"

#include "test_compilation_error.hpp"

using namespace wuzp;  // NOLINT

TEST(TestQuantityMatrix2, ConstructFromGoodQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = UnitMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  MyQuantityMatrix myMatrix(
      QuantityTraits::Create<Quantity<int, Length>>::create(1), QuantityTraits::Create<Quantity<int, Area>>::create(2),
      QuantityTraits::Create<Quantity<int, Volume>>::create(3), QuantityTraits::Create<Quantity<int, Mass>>::create(4));
}

// TEST(TestQuantityMatrix2, ConstructFromBadQuantities) {  // NOLINT
//  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
//#include "wuzp/core/QuantityMatrix.hpp"
//#include "wuzp/units/Quantity.hpp"
//#include "wuzp/units/QuantityTraits.hpp"
//#include "wuzp/units/Unit.hpp"
//#include "wuzp/units/UnitMatrix.hpp"
//#include "wuzp/value_matrix/ValueMatrix.hpp"
//
// using namespace wuzp;  // NOLINT
//
// int main() {
//  using MyValueMatrix = ValueMatrix<int, 4, 1>;
//  using MyUnitMatrix = UnitMatrix<4, 1, Length, Area, Volume, Mass>;
//  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
//  MyQuantityMatrix myMatrix(QuantityTraits::Create<Quantity<int, Length>>::create(1),
//                    QuantityTraits::Create<Quantity<int, Area>>::create(2),
//                    QuantityTraits::Create<Quantity<int, Volume>>::create(3),
//                    QuantityTraits::Create<Quantity<int, Length>>::create(4));
//  return 0;
//}
//  )",
//                                               "Units do not match.")
//}

class TestQuantityMatrix : public ::testing::Test {
 public:
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = UnitMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;

  const MyValueMatrix& getValues1() const { return valueMatrix1_; }
  const MyValueMatrix& getValues2() const { return valueMatrix2_; }
  const MyQuantityMatrix& getMatrix1() const { return quantityMatrix1_; }
  const MyQuantityMatrix& getMatrix2() const { return quantityMatrix2_; }
  MyQuantityMatrix& getMatrix1() { return quantityMatrix1_; }

 protected:
  void SetUp() override {
    valueMatrix1_ << 1, 2, 3, 4, 5, 6;
    quantityMatrix1_.setValueMatrix(valueMatrix1_);
    valueMatrix2_ << 3, 1, 4, 1, 5, 9;
    quantityMatrix2_.setValueMatrix(valueMatrix2_);
  }

 private:
  MyValueMatrix valueMatrix1_;
  MyQuantityMatrix quantityMatrix1_;
  MyValueMatrix valueMatrix2_;
  MyQuantityMatrix quantityMatrix2_;
};

TEST_F(TestQuantityMatrix, SetGetValueMatrix) {  // NOLINT
  EXPECT_EQ(getMatrix1().getValueMatrix(), getValues1());
  getMatrix1().setValueMatrix(getValues2());
  EXPECT_EQ(getMatrix1().getValueMatrix(), getValues2());
}

TEST_F(TestQuantityMatrix, SetGetQuantity) {  // NOLINT
  using EnergyQuantity = Quantity<int, Energy>;
  EXPECT_EQ((getMatrix1().getQuantity<1, 2>()), QuantityTraits::Create<EnergyQuantity>::create(getValues1()(1, 2)));
  const auto quantity = QuantityTraits::Create<EnergyQuantity>::create(7);
  getMatrix1().setQuantity<1, 2>(quantity);
  EXPECT_EQ((getMatrix1().getQuantity<1, 2>()), quantity);
}

// TEST(TestQuantityMatrixStandalone, SetBadQuantity){// NOLINT
//                                                   ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
//#include "wuzp/core/QuantityMatrix.hpp"
//#include "wuzp/units/Quantity.hpp"
//#include "wuzp/units/QuantityTraits.hpp"
//#include "wuzp/units/Unit.hpp"
//#include "wuzp/units/UnitMatrix.hpp"
//#include "wuzp/value_matrix/ValueMatrix.hpp"
//
// using namespace wuzp;  // NOLINT
//
// int main() {
//  using MyValueMatrix = ValueMatrix<int, 2, 3>;
//  using MyUnitMatrix = UnitMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
//  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
//  MyQuantityMatrix quantityMatrix;
//  quantityMatrix.setQuantity<1, 2>(QuantityTraits::Create<Quantity<int, Time>>::create(7));
//  return 0;
//}
//  )",
//                                                                                                "Units do not match.")}

TEST_F(TestQuantityMatrix, Addition) {  // NOLINT
  EXPECT_EQ(getMatrix1() + getMatrix2(), MyQuantityMatrix(getValues1() + getValues2()));
}

TEST_F(TestQuantityMatrix, Subtraction) {  // NOLINT
  EXPECT_EQ(getMatrix1() - getMatrix2(), MyQuantityMatrix(getValues1() - getValues2()));
}

TEST_F(TestQuantityMatrix, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getMatrix1(), getMatrix1());
}

TEST_F(TestQuantityMatrix, InequalityOperator) {  // NOLINT
  EXPECT_NE(getMatrix1(), getMatrix1() + getMatrix2());
}

TEST_F(TestQuantityMatrix, Block) {  // NOLINT
  using MyValueMatrixB = ValueMatrix<int, 2, 2>;
  using MyUnitMatrixB = UnitMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyQuantityMatrixB = QuantityMatrix<MyValueMatrixB, MyUnitMatrixB>;
  MyValueMatrixB valueMatrixB;
  valueMatrixB << 2, 3, 5, 6;
  MyQuantityMatrixB quantityMatrixB(valueMatrixB);
  EXPECT_EQ((getMatrix1().block<0, 1, 2, 2>()), quantityMatrixB);
}

TEST_F(TestQuantityMatrix, Transpose) {  // NOLINT
  using MyValueMatrixT = ValueMatrix<int, 3, 2>;
  using MyUnitMatrixT = UnitMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyQuantityMatrixT = QuantityMatrix<MyValueMatrixT, MyUnitMatrixT>;
  MyValueMatrixT valueMatrixT;
  valueMatrixT << 1, 4, 2, 5, 3, 6;
  MyQuantityMatrixT quantityMatrixT(valueMatrixT);
  EXPECT_EQ(getMatrix1().transpose(), quantityMatrixT);
}

TEST(TestQuantityMatrix2, ProductQuantityMatrix) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = UnitMatrix<2, 2, Length>;
  using MyUnitMatrixResult = UnitMatrix<2, 2, Area>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
  MyValueMatrix valueMatrix;
  valueMatrix << 1, 2, 3, 4;
  MyQuantityMatrix quantityMatrix;
  quantityMatrix.setValueMatrix(valueMatrix);
  EXPECT_EQ(quantityMatrix * quantityMatrix, MyQuantityMatrixResult(valueMatrix * valueMatrix));
}

TEST(TestQuantityMatrix2, ProductQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = UnitMatrix<2, 2, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyValue = int;
  using MyQuantity = Quantity<MyValue, Length>;
  using MyUnitMatrixResult = UnitMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
  const MyValue value{2};
  const auto quantity = QuantityTraits::Create<MyQuantity>::create(value);
  MyValueMatrix valueMatrix;
  valueMatrix << 1, 2, 3, 4;
  MyQuantityMatrix quantityMatrix;
  quantityMatrix.setValueMatrix(valueMatrix);
  EXPECT_EQ(quantityMatrix * quantity, MyQuantityMatrixResult(valueMatrix * value));
  EXPECT_EQ(quantity * quantityMatrix, MyQuantityMatrixResult(valueMatrix * value));
}

TEST(TestQuantityMatrix2, DivisionQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = UnitMatrix<2, 2, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyValue = int;
  using MyQuantity = Quantity<MyValue, Length>;
  using MyUnitMatrixResult = UnitMatrix<2, 2, Dimensionless>;
  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
  const MyValue value{2};
  const auto quantity = QuantityTraits::Create<MyQuantity>::create(value);
  MyValueMatrix valueMatrix;
  valueMatrix << 1, 2, 3, 4;
  MyQuantityMatrix quantityMatrix;
  quantityMatrix.setValueMatrix(valueMatrix);
  EXPECT_EQ(quantityMatrix / quantity, MyQuantityMatrixResult(valueMatrix / value));
}

TEST(TestQuantityMatrix2, ScalarProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = UnitMatrix<4, 1, Length>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;
  using MyQuantityResult = Quantity<int, Area>;
  MyValueMatrix valueMatrix1;
  valueMatrix1 << 1, 2, 3, 4;
  MyValueMatrix valueMatrix2;
  valueMatrix2 << 0, 3, -1, 2;
  EXPECT_EQ(MyQuantityMatrix(valueMatrix1).dot(MyQuantityMatrix(valueMatrix2)), QuantityTraits::Create<MyQuantityResult>::create(11));
}

TEST(TestQuantityMatrix2, Chain) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix1 = UnitMatrix<2, 2, Length>;
  using MyUnitMatrix2 = UnitMatrix<2, 2, Area>;
  using MyUnitMatrixResult = UnitMatrix<2, 2, Area>;
  using MyQuantityMatrix1 = QuantityMatrix<MyValueMatrix, MyUnitMatrix1>;
  using MyQuantityMatrix2 = QuantityMatrix<MyValueMatrix, MyUnitMatrix2>;
  using MyQuantityMatrixResult = QuantityMatrix<MyValueMatrix, MyUnitMatrixResult>;
  MyValueMatrix valueMatrix;
  valueMatrix << 1, 2, 3, 4;
  MyQuantityMatrix1 quantityMatrix1(valueMatrix);
  MyQuantityMatrix2 quantityMatrix2(valueMatrix);
  EXPECT_EQ(quantityMatrix2 + quantityMatrix1 * quantityMatrix1, MyQuantityMatrixResult(valueMatrix + valueMatrix * valueMatrix));
}

class TestQuantityMatrixInvertible : public ::testing::Test {
 public:
  using MyValueMatrix = ValueMatrix<double, 2, 2>;
  using MyUnitMatrix = UnitMatrix<2, 2, Time>;
  using MyQuantityMatrix = QuantityMatrix<MyValueMatrix, MyUnitMatrix>;

  const MyValueMatrix& getValues() const { return valueMatrix_; }
  const MyQuantityMatrix& getMatrix() const { return quantityMatrix_; }

 protected:
  void SetUp() override {
    valueMatrix_ << 4.0, 7.0, 2.0, 6.0;
    quantityMatrix_.setValueMatrix(valueMatrix_);
  }

 private:
  MyValueMatrix valueMatrix_;
  MyQuantityMatrix quantityMatrix_;
};

TEST_F(TestQuantityMatrixInvertible, Inverse) {  // NOLINT
  using MyUnitMatrixInv = UnitMatrix<2, 2, Frequency>;
  using MyQuantityMatrixInv = QuantityMatrix<MyValueMatrix, MyUnitMatrixInv>;
  MyValueMatrix valueMatrixInv;
  valueMatrixInv << 0.6, -0.7, -0.2, 0.4;
  MyQuantityMatrixInv quantityMatrixInv(valueMatrixInv);
  EXPECT_TRUE(getMatrix().inverse().isApprox(quantityMatrixInv));
}
