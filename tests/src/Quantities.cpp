#include <gtest/gtest.h>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "test_compilation_error.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/Values.hpp"
using namespace wuzp_eigen;  // NOLINT

using namespace wuzp;  // NOLINT

template <int Rows, int Cols>
using TestValues = Values<double, Rows, Cols>;
template <int Rows, int Cols>
using TestUnitMatrix = TypeMatrix<Rows, Cols, Length>;
template <int Rows, int Cols>
using TestQuantityMatrix = Quantities<TestValues<Rows, Cols>, TestUnitMatrix<Rows, Cols>>;

using LengthQuantityI = QuantityI<Length>;
using AreaQuantityI = QuantityI<Area>;

TEST(TestQuantityMatrix2, Dimension) {  // NOLINT
  static_assert(Dimension<TestQuantityMatrix<1, 1>>::value == 0);
  static_assert(Dimension<TestQuantityMatrix<3, 1>>::value == 1);
  static_assert(Dimension<TestQuantityMatrix<1, 2>>::value == 1);
  static_assert(Dimension<TestQuantityMatrix<3, 2>>::value == 2);
}

TEST(TestQuantityMatrix2, Rows) {  // NOLINT
  static_assert(Rows<TestQuantityMatrix<3, 2>>::value == 3);
}

TEST(TestQuantityMatrix2, Cols) {  // NOLINT
  static_assert(Cols<TestQuantityMatrix<3, 2>>::value == 2);
}

TEST(TestQuantityMatrix2, GetTag) {  // NOLINT
  using MyValues = Values<int, 4, 1>;
  using MyValue = Values<int, 1, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, MyQuantityMatrix>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, const MyQuantityMatrix&>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, MyQuantityMatrix&&>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, Quantity<MyValue, Length>>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, Quantity<MyValue, Length>, Quantity<MyValue, Area>, Quantity<MyValue, Volume>,
                                      Quantity<MyValue, Mass>>::type,
                               FromImplementation>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, MyValues>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, double>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, double, int, float, double>::type, FromImplementationArgs>);
}

TEST(TestQuantityMatrix2, EmptyConstructor) {  // NOLINT
  using MyValues = Values<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  [[maybe_unused]] const MyQuantityMatrix myMatrix;
}

TEST(TestQuantityMatrix2, ConstructImplicityFromQuantityMatrix) {  // NOLINT
  using MyValues = Values<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  const MyQuantityMatrix quantityMatrix1;
  const MyQuantityMatrix quantityMatrix2;
  const auto quantityMatrix3 = quantityMatrix1 + quantityMatrix2;
  const MyQuantityMatrix quantityMatrix = quantityMatrix3;
  EXPECT_EQ(quantityMatrix, quantityMatrix3);
}

TEST(TestQuantityMatrix2, ConstructImplicitlyFromValues){// NOLINT
                                                         ASSERT_COMPILATION_ERROR_WITH_MESSAGE(R"(
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

using namespace wuzp;  // NOLINT

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/Values.hpp"
template <typename ValueT, int Rows, int Cols>
using Values = wuzp_eigen::Values<ValueT, Rows, Cols>;
#endif

using MyValues = Values<int, 2, 3>;
using MyUnitMatrix = TypeMatrix<2, 3, Length>;
using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;

int main() {
  const MyValues values(1, 2, 3, 4, 5, 6);
  [[maybe_unused]] const MyQuantityMatrix quantityMatrix = values;
  return 0;
}
)",
                                                                                               "(no viable )?conversion from")}

TEST(TestQuantityMatrix2, ConstructExplicitlyFromQuantities) {  // NOLINT
  using MyValues = Values<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  using MyQuantity = LengthQuantityI;
  const MyQuantityMatrix quantityMatrix(MyQuantity(1), MyQuantity(2), MyQuantity(3), MyQuantity(4), MyQuantity(5), MyQuantity(6));
  const MyValues values(1, 2, 3, 4, 5, 6);
  EXPECT_EQ(quantityMatrix.getValues(), values);
}

TEST(TestQuantityMatrix2, ConstructExplicitlyFromValues) {  // NOLINT
  using MyValues = Values<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  const MyValues values(1, 2, 3, 4, 5, 6);
  const MyQuantityMatrix quantityMatrix(values);
  EXPECT_EQ(quantityMatrix.getValues(), values);
}

TEST(TestQuantityMatrix2, ConstructExplicitlyFromCsv) {  // NOLINT
  using MyValues = Values<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  const MyValues values(1, 2, 3, 4, 5, 6);
  const MyQuantityMatrix quantityMatrix(1, 2, 3, 4, 5, 6);
  EXPECT_EQ(quantityMatrix.getValues(), values);
}

// TODO(remod): Support construction from const values.
// TEST(TestQuantityMatrix2, ConstructExplicitlyFromConstValues) {  // NOLINT
//  using MyValues = Values<int, 2, 3>;
//  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
//  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
//  MyValues values;
//  values << 1, 2, 3, 4, 5, 6;
//  const int v1 = 1;
//  const int v2 = 2;
//  const int v3 = 3;
//  const int v4 = 4;
//  const int v5 = 5;
//  const int v6 = 6;
//  const MyQuantityMatrix quantityMatrix(v1, v2, v3, v4, v5, v6);
//  EXPECT_EQ(quantityMatrix.getValues(), values);
//}

TEST(TestQuantityMatrix2, ConstructFromGoodQuantities) {  // NOLINT
  using MyValues = Values<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  MyQuantityMatrix quantityMatrix(Quantity<ValueI, Length>(1), Quantity<ValueI, Area>(2), Quantity<ValueI, Volume>(3),
                                  Quantity<ValueI, Mass>(4));
}

TEST(TestQuantityMatrix2, ConstructFromBadQuantities) {  // NOLINT
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Unit.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/Values.hpp"
template <typename ValueT, int Rows, int Cols>
using Values = wuzp_eigen::Values<ValueT, Rows, Cols>;
#endif

using namespace wuzp;  // NOLINT

int main() {
  using MyValues = Values<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  MyQuantityMatrix quantityMatrix(QuantityI<Length>(1),
                                  QuantityI<Area>(2),
                                  QuantityI<Volume>(3),
                                  QuantityI<Length>(4));
  return 0;
}
  )",
                                               "Units do not match.")
}

class TestQuantityMatrixFixture : public ::testing::Test {
 public:
  using MyValues = ValuesI<2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;

  const MyValues& getValues1() const { return values1_; }
  const MyValues& getValues2() const { return values2_; }
  const MyQuantityMatrix& getQuantityMatrix1() const { return quantityMatrix1_; }
  const MyQuantityMatrix& getQuantityMatrix2() const { return quantityMatrix2_; }
  MyQuantityMatrix& getQuantityMatrix1() { return quantityMatrix1_; }

 protected:
  void SetUp() override {
    quantityMatrix1_.setValues(values1_);
    quantityMatrix2_.setValues(values2_);
  }

 private:
  MyValues values1_{1, 2, 3, 4, 5, 6};
  MyQuantityMatrix quantityMatrix1_;
  MyValues values2_{3, 1, 4, 1, 5, 9};
  MyQuantityMatrix quantityMatrix2_;
};

TEST_F(TestQuantityMatrixFixture, SetGetValues) {  // NOLINT
  EXPECT_EQ(getQuantityMatrix1().getValues(), getValues1());
  getQuantityMatrix1().setValues(getValues2());
  EXPECT_EQ(getQuantityMatrix1().getValues(), getValues2());
}

TEST_F(TestQuantityMatrixFixture, SetGetQuantity) {  // NOLINT
  using EnergyQuantity = QuantityI<Energy>;
  EXPECT_EQ((getQuantityMatrix1().get<1, 2>()), EnergyQuantity(getValues1().get<1, 2>()));
  const auto quantity = EnergyQuantity(7);
  getQuantityMatrix1().set<1, 2>(quantity);
  EXPECT_EQ((getQuantityMatrix1().get<1, 2>()), quantity);
}

TEST(TestQuantityMatrixStandalone, SetBadQuantity){// NOLINT
                                                   ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Unit.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/Values.hpp"
template <typename ValueT, int Rows, int Cols>
using Values = wuzp_eigen::Values<ValueT, Rows, Cols>;
#endif

using namespace wuzp;  // NOLINT

int main() {
  using MyValues = Values<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  MyQuantityMatrix quantityMatrix;
  quantityMatrix.set<1, 2>(QuantityI<Time>(7));
  return 0;
}
  )",
                                                                                                "Units do not match.")}

TEST_F(TestQuantityMatrixFixture, Addition) {  // NOLINT
  EXPECT_EQ(getQuantityMatrix1() + getQuantityMatrix2(), MyQuantityMatrix(getValues1() + getValues2()));
}

TEST_F(TestQuantityMatrixFixture, Subtraction) {  // NOLINT
  EXPECT_EQ(getQuantityMatrix1() - getQuantityMatrix2(), MyQuantityMatrix(getValues1() - getValues2()));
}

TEST_F(TestQuantityMatrixFixture, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getQuantityMatrix1(), getQuantityMatrix1());
}

TEST_F(TestQuantityMatrixFixture, InequalityOperator) {  // NOLINT
  EXPECT_NE(getQuantityMatrix1(), getQuantityMatrix1() + getQuantityMatrix2());
}

TEST_F(TestQuantityMatrixFixture, Block) {  // NOLINT
  using MyValuesB = Values<int, 2, 2>;
  using MyUnitMatrixB = TypeMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyQuantityMatrixB = Quantities<MyValuesB, MyUnitMatrixB>;
  const MyValuesB valuesB(2, 3, 5, 6);
  const MyQuantityMatrixB quantityMatrixB(valuesB);
  EXPECT_EQ((getQuantityMatrix1().block<0, 1, 2, 2>()), quantityMatrixB);
}

TEST_F(TestQuantityMatrixFixture, Transpose) {  // NOLINT
  using MyValuesT = Values<int, 3, 2>;
  using MyUnitMatrixT = TypeMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyQuantityMatrixT = Quantities<MyValuesT, MyUnitMatrixT>;
  const MyValuesT valuesT(1, 4, 2, 5, 3, 6);
  const MyQuantityMatrixT quantityMatrixT(valuesT);
  EXPECT_EQ(getQuantityMatrix1().transpose(), quantityMatrixT);
}

TEST(TestQuantityMatrix2, ProductQuantityMatrix) {  // NOLINT
  using MyValues = Values<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  using MyQuantityMatrixResult = Quantities<MyValues, MyUnitMatrixResult>;
  const MyValues values(1, 2, 3, 4);
  const MyQuantityMatrix quantityMatrix(values);
  EXPECT_EQ(quantityMatrix * quantityMatrix, MyQuantityMatrixResult(values * values));
}

TEST(TestQuantityMatrix2, ProductQuantity) {  // NOLINT
  using MyValues = Values<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  using MyQuantity = QuantityI<Length>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = Quantities<MyValues, MyUnitMatrixResult>;
  const int value{2};
  const auto quantity = MyQuantity(value);
  const MyValues values(1, 2, 3, 4);
  const MyQuantityMatrix quantityMatrix(values);
  EXPECT_EQ(quantityMatrix * quantity, MyQuantityMatrixResult(values * value));
  EXPECT_EQ(quantity * quantityMatrix, MyQuantityMatrixResult(values * value));
}

TEST(TestQuantityMatrix2, ProductValue) {  // NOLINT
  using MyValues = Values<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  using MyValue = int;
  const MyValue value{2};
  const MyValues values(1, 2, 3, 4);
  const MyQuantityMatrix quantityMatrix(values);
  EXPECT_EQ(quantityMatrix * value, MyQuantityMatrix(values * value));
  EXPECT_EQ(value * quantityMatrix, MyQuantityMatrix(values * value));
}

TEST(TestQuantityMatrix2, DivisionQuantity) {  // NOLINT
  using MyValues = Values<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  using MyValue = int;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Dimensionless>;
  using MyQuantityMatrixResult = Quantities<MyValues, MyUnitMatrixResult>;
  const MyValue value{2};
  const auto quantity = LengthQuantityI(value);
  const MyValues values(1, 2, 3, 4);
  const MyQuantityMatrix quantityMatrix(values);
  EXPECT_EQ(quantityMatrix / quantity, MyQuantityMatrixResult(values / value));
}

TEST(TestQuantityMatrix2, DivisionValue) {  // NOLINT
  using MyValues = Values<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  using MyValue = int;
  const MyValue value{2};
  const MyValues values(1, 2, 3, 4);
  const MyQuantityMatrix quantityMatrix(values);
  EXPECT_EQ(quantityMatrix / value, MyQuantityMatrix(values / value));
}

TEST(TestQuantityMatrix2, Dot) {  // NOLINT
  using MyValues = Values<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;
  using MyQuantityResult = AreaQuantityI;
  const MyValues values1(1, 2, 3, 4);
  const MyValues values2(0, 3, -1, 2);
  EXPECT_EQ(MyQuantityMatrix(values1).dotProduct(MyQuantityMatrix(values2)), MyQuantityResult(11));
}

TEST(TestQuantityMatrix2, Chain) {  // NOLINT
  using MyValues = Values<int, 2, 2>;
  using MyUnitMatrix1 = TypeMatrix<2, 2, Length>;
  using MyUnitMatrix2 = TypeMatrix<2, 2, Area>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrix1 = Quantities<MyValues, MyUnitMatrix1>;
  using MyQuantityMatrix2 = Quantities<MyValues, MyUnitMatrix2>;
  using MyQuantityMatrixResult = Quantities<MyValues, MyUnitMatrixResult>;
  const MyValues values(1, 2, 3, 4);
  const MyQuantityMatrix1 quantityMatrix1(values);
  const MyQuantityMatrix2 quantityMatrix2(values);
  EXPECT_EQ(quantityMatrix2 + quantityMatrix1 * quantityMatrix1, MyQuantityMatrixResult(values + values * values));
}

class TestQuantityMatrixInvertible : public ::testing::Test {
 public:
  using MyValues = Values<double, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Time>;
  using MyQuantityMatrix = Quantities<MyValues, MyUnitMatrix>;

  const MyValues& getValues() const { return values_; }
  const MyQuantityMatrix& getQuantityMatrix() const { return quantityMatrix_; }

 protected:
  void SetUp() override { quantityMatrix_.setValues(values_); }

 private:
  MyValues values_{4.0, 7.0, 2.0, 6.0};
  MyQuantityMatrix quantityMatrix_;
};

TEST_F(TestQuantityMatrixInvertible, Inverse) {  // NOLINT
  using MyUnitMatrixInv = TypeMatrix<2, 2, Frequency>;
  using MyQuantityMatrixInv = Quantities<MyValues, MyUnitMatrixInv>;
  const MyValues valuesInv{0.6, -0.7, -0.2, 0.4};
  const MyQuantityMatrixInv quantityMatrixInv(valuesInv);
  EXPECT_TRUE(getQuantityMatrix().inverse().isApprox(quantityMatrixInv));
}

#endif
