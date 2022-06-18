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
using TestQuantities = Quantities<TestValues<Rows, Cols>, TestUnitMatrix<Rows, Cols>>;

using LengthQuantityI = QuantityI<Length>;
using AreaQuantityI = QuantityI<Area>;

TEST(TestQuantities2, Dimension) {  // NOLINT
  static_assert(Dimension<TestQuantities<1, 1>>::value == 0);
  static_assert(Dimension<TestQuantities<3, 1>>::value == 1);
  static_assert(Dimension<TestQuantities<1, 2>>::value == 1);
  static_assert(Dimension<TestQuantities<3, 2>>::value == 2);
}

TEST(TestQuantities2, Rows) {  // NOLINT
  static_assert(Rows<TestQuantities<3, 2>>::value == 3);
}

TEST(TestQuantities2, Cols) {  // NOLINT
  static_assert(Cols<TestQuantities<3, 2>>::value == 2);
}

TEST(TestQuantities2, GetTag) {  // NOLINT
  using MyValues = Values<int, 4, 1>;
  using MyValue = Values<int, 1, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  static_assert(std::is_same_v<GetTag<MyQuantities, MyQuantities>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyQuantities, const MyQuantities&>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyQuantities, MyQuantities&&>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyQuantities, Quantity<MyValue, Length>>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyQuantities, Quantity<MyValue, Length>, Quantity<MyValue, Area>, Quantity<MyValue, Volume>,
                                      Quantity<MyValue, Mass>>::type,
                               FromImplementation>);
  static_assert(std::is_same_v<GetTag<MyQuantities, MyValues>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyQuantities, double>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyQuantities, double, int, float, double>::type, FromImplementationArgs>);
}

TEST(TestQuantities2, EmptyConstructor) {  // NOLINT
  using MyValues = Values<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  [[maybe_unused]] const MyQuantities myMatrix;
}

TEST(TestQuantities2, ConstructImplicityFromQuantities) {  // NOLINT
  using MyValues = Values<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  const MyQuantities quantities1;
  const MyQuantities quantities2;
  const MyQuantities quantities3 = quantities1 + quantities2;
  const MyQuantities quantities = quantities3;
  EXPECT_EQ(quantities, quantities3);
}

TEST(TestQuantities2, ConstructImplicitlyFromValues){// NOLINT
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
using MyQuantities = Quantities<MyValues, MyUnitMatrix>;

int main() {
  const MyValues values(1, 2, 3, 4, 5, 6);
  [[maybe_unused]] const MyQuantities quantities = values;
  return 0;
}
)",
                                                                                           "(no viable )?conversion from")}

TEST(TestQuantities2, ConstructExplicitlyFromQuantities) {  // NOLINT
  using MyValues = Values<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  using MyQuantity = LengthQuantityI;
  const MyQuantities quantities(MyQuantity(1), MyQuantity(2), MyQuantity(3), MyQuantity(4), MyQuantity(5), MyQuantity(6));
  const MyValues values(1, 2, 3, 4, 5, 6);
  EXPECT_EQ(quantities.getValues(), values);
}

TEST(TestQuantities2, ConstructExplicitlyFromValues) {  // NOLINT
  using MyValues = Values<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  const MyValues values(1, 2, 3, 4, 5, 6);
  const MyQuantities quantities(values);
  EXPECT_EQ(quantities.getValues(), values);
}

TEST(TestQuantities2, ConstructExplicitlyFromCsv) {  // NOLINT
  using MyValues = Values<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  const MyValues values(1, 2, 3, 4, 5, 6);
  const MyQuantities quantities(1, 2, 3, 4, 5, 6);
  EXPECT_EQ(quantities.getValues(), values);
}

TEST(TestQuantities2, ConstructExplicitlyFromConstValues) {  // NOLINT
  using MyValues = Values<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  MyValues values(1, 2, 3, 4, 5, 6);
  const int v1 = 1;
  const int v2 = 2;
  const int v3 = 3;
  const int v4 = 4;
  const int v5 = 5;
  const int v6 = 6;
  const MyQuantities quantities(v1, v2, v3, v4, v5, v6);
  EXPECT_EQ(quantities.getValues(), values);
}

TEST(TestQuantities2, ConstructFromGoodQuantities) {  // NOLINT
  using MyValues = Values<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  MyQuantities quantities(Quantity<ValueI, Length>(1), Quantity<ValueI, Area>(2), Quantity<ValueI, Volume>(3), Quantity<ValueI, Mass>(4));
}

TEST(TestQuantities2, ConstructFromBadQuantities) {  // NOLINT
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
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  MyQuantities quantities(QuantityI<Length>(1),
                                  QuantityI<Area>(2),
                                  QuantityI<Volume>(3),
                                  QuantityI<Length>(4));
  return 0;
}
  )",
                                               "Units do not match.")
}

class TestQuantitiesFixture : public ::testing::Test {
 public:
  using MyValues = ValuesI<2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;

  const MyValues& getValues1() const { return values1_; }
  const MyValues& getValues2() const { return values2_; }
  const MyQuantities& getQuantities1() const { return quantities1_; }
  const MyQuantities& getQuantities2() const { return quantities2_; }
  MyQuantities& getQuantities1() { return quantities1_; }

 protected:
  void SetUp() override {
    quantities1_.setValues(values1_);
    quantities2_.setValues(values2_);
  }

 private:
  MyValues values1_{1, 2, 3, 4, 5, 6};
  MyQuantities quantities1_;
  MyValues values2_{3, 1, 4, 1, 5, 9};
  MyQuantities quantities2_;
};

TEST_F(TestQuantitiesFixture, SetGetValues) {  // NOLINT
  EXPECT_EQ(getQuantities1().getValues(), getValues1());
  getQuantities1().setValues(getValues2());
  EXPECT_EQ(getQuantities1().getValues(), getValues2());
}

TEST_F(TestQuantitiesFixture, SetGetQuantity) {  // NOLINT
  using EnergyQuantity = QuantityI<Energy>;
  EXPECT_EQ((getQuantities1().get<1, 2>()), EnergyQuantity(getValues1().get<1, 2>()));
  const EnergyQuantity quantity(7);
  getQuantities1().set<1, 2>(quantity);
  EXPECT_EQ((getQuantities1().get<1, 2>()), quantity);
}

TEST(TestQuantitiesStandalone, SetBadQuantity){// NOLINT
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
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  MyQuantities quantities;
  quantities.set<1, 2>(QuantityI<Time>(7));
  return 0;
}
  )",
                                                                                            "Units do not match.")}

TEST_F(TestQuantitiesFixture, Addition) {  // NOLINT
  EXPECT_EQ(getQuantities1() + getQuantities2(), MyQuantities(getValues1() + getValues2()));
}

TEST_F(TestQuantitiesFixture, Subtraction) {  // NOLINT
  EXPECT_EQ(getQuantities1() - getQuantities2(), MyQuantities(getValues1() - getValues2()));
}

TEST_F(TestQuantitiesFixture, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getQuantities1(), getQuantities1());
}

TEST_F(TestQuantitiesFixture, InequalityOperator) {  // NOLINT
  EXPECT_NE(getQuantities1(), getQuantities1() + getQuantities2());
}

TEST_F(TestQuantitiesFixture, Block) {  // NOLINT
  using MyValuesB = Values<int, 2, 2>;
  using MyUnitMatrixB = TypeMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyQuantitiesB = Quantities<MyValuesB, MyUnitMatrixB>;
  const MyValuesB valuesB(2, 3, 5, 6);
  const MyQuantitiesB quantitiesB(valuesB);
  EXPECT_EQ((getQuantities1().block<0, 1, 2, 2>()), quantitiesB);
}

TEST_F(TestQuantitiesFixture, Transpose) {  // NOLINT
  using MyValuesT = Values<int, 3, 2>;
  using MyUnitMatrixT = TypeMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyQuantitiesT = Quantities<MyValuesT, MyUnitMatrixT>;
  const MyValuesT valuesT(1, 4, 2, 5, 3, 6);
  const MyQuantitiesT quantitiesT(valuesT);
  EXPECT_EQ(getQuantities1().transpose(), quantitiesT);
}

TEST(TestQuantities2, ProductQuantities) {  // NOLINT
  using MyValues = Values<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  using MyQuantitiesResult = Quantities<MyValues, MyUnitMatrixResult>;
  const MyValues values(1, 2, 3, 4);
  const MyQuantities quantities(values);
  EXPECT_EQ(quantities * quantities, MyQuantitiesResult(values * values));
}

TEST(TestQuantities2, ProductQuantity) {  // NOLINT
  using MyValues = Values<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  using MyQuantity = QuantityI<Length>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantitiesResult = Quantities<MyValues, MyUnitMatrixResult>;
  const int value{2};
  const MyQuantity quantity(value);
  const MyValues values(1, 2, 3, 4);
  const MyQuantities quantities(values);
  EXPECT_EQ(quantities * quantity, MyQuantitiesResult(values * value));
  EXPECT_EQ(quantity * quantities, MyQuantitiesResult(values * value));
}

TEST(TestQuantities2, ProductValue) {  // NOLINT
  using MyValues = Values<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  using MyValue = int;
  const MyValue value{2};
  const MyValues values(1, 2, 3, 4);
  const MyQuantities quantities(values);
  EXPECT_EQ(quantities * value, MyQuantities(values * value));
  EXPECT_EQ(value * quantities, MyQuantities(values * value));
}

TEST(TestQuantities2, DivisionQuantity) {  // NOLINT
  using MyValues = Values<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  using MyValue = int;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Dimensionless>;
  using MyQuantitiesResult = Quantities<MyValues, MyUnitMatrixResult>;
  const MyValue value{2};
  const LengthQuantityI quantity(value);
  const MyValues values(1, 2, 3, 4);
  const MyQuantities quantities(values);
  EXPECT_EQ(quantities / quantity, MyQuantitiesResult(values / value));
}

TEST(TestQuantities2, DivisionValue) {  // NOLINT
  using MyValues = Values<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  using MyValue = int;
  const MyValue value{2};
  const MyValues values(1, 2, 3, 4);
  const MyQuantities quantities(values);
  EXPECT_EQ(quantities / value, MyQuantities(values / value));
}

TEST(TestQuantities2, Dot) {  // NOLINT
  using MyValues = Values<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;
  using MyQuantityResult = AreaQuantityI;
  const MyValues values1(1, 2, 3, 4);
  const MyValues values2(0, 3, -1, 2);
  EXPECT_EQ(MyQuantities(values1).dotProduct(MyQuantities(values2)), MyQuantityResult(11));
}

TEST(TestQuantities2, Chain) {  // NOLINT
  using MyValues = Values<int, 2, 2>;
  using MyUnitMatrix1 = TypeMatrix<2, 2, Length>;
  using MyUnitMatrix2 = TypeMatrix<2, 2, Area>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantities1 = Quantities<MyValues, MyUnitMatrix1>;
  using MyQuantities2 = Quantities<MyValues, MyUnitMatrix2>;
  using MyQuantitiesResult = Quantities<MyValues, MyUnitMatrixResult>;
  const MyValues values(1, 2, 3, 4);
  const MyQuantities1 quantities1(values);
  const MyQuantities2 quantities2(values);
  EXPECT_EQ(quantities2 + quantities1 * quantities1, MyQuantitiesResult(values + values * values));
}

class TestQuantitiesInvertible : public ::testing::Test {
 public:
  using MyValues = Values<double, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Time>;
  using MyQuantities = Quantities<MyValues, MyUnitMatrix>;

  const MyValues& getValues() const { return values_; }
  const MyQuantities& getQuantities() const { return quantities_; }

 protected:
  void SetUp() override { quantities_.setValues(values_); }

 private:
  MyValues values_{4.0, 7.0, 2.0, 6.0};
  MyQuantities quantities_;
};

TEST_F(TestQuantitiesInvertible, Inverse) {  // NOLINT
  using MyUnitMatrixInv = TypeMatrix<2, 2, Frequency>;
  using MyQuantitiesInv = Quantities<MyValues, MyUnitMatrixInv>;
  const MyValues valuesInv{0.6, -0.7, -0.2, 0.4};
  const MyQuantitiesInv quantitiesInv(valuesInv);
  EXPECT_TRUE(getQuantities().inverse().isApprox(quantitiesInv));
}

#endif
