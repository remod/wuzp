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
#include "wuzp_eigen/ValueMatrix.hpp"
using namespace wuzp_eigen;  // NOLINT

using namespace wuzp;  // NOLINT

template <int Rows, int Cols>
using TestValues = ValueMatrix<double, Rows, Cols>;
template <int Rows, int Cols>
using TestUnitMatrix = TypeMatrix<Rows, Cols, Length>;
template <int Rows, int Cols>
using TestQuantities = Quantities<TestValues<Rows, Cols>, TestUnitMatrix<Rows, Cols>>;

using LengthQuantityI = QuantityI<Length>;
using SquaredLengthQuantityI = QuantityI<Power<Length, Integer<2>>::type>;
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
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyValue = ValueMatrix<int, 1, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, MyQuantityMatrix>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, const MyQuantityMatrix&>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, MyQuantityMatrix&&>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, Quantity<MyValue, Length>>::type, FromSimilar>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, Quantity<MyValue, Length>, Quantity<MyValue, Area>, Quantity<MyValue, Volume>,
                                      Quantity<MyValue, Mass>>::type,
                               FromImplementation>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, MyValueMatrix>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, double>::type, FromImplementationArgs>);
  static_assert(std::is_same_v<GetTag<MyQuantityMatrix, double, int, float, double>::type, FromImplementationArgs>);
}

TEST(TestQuantities2, EmptyConstructor) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  [[maybe_unused]] const MyQuantityMatrix myMatrix;
}

TEST(TestQuantities2, ConstructImplicityFromQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyQuantityMatrix quantities1;
  const MyQuantityMatrix quantities2;
  const MyQuantityMatrix quantities3 = quantities1 + quantities2;
  const MyQuantityMatrix quantities = quantities3;
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
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<ValueT, Rows, Cols>;
#endif

using MyValueMatrix = ValueMatrix<int, 2, 3>;
using MyUnitMatrix = TypeMatrix<2, 3, Length>;
using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;

int main() {
  const MyValueMatrix values(1, 2, 3, 4, 5, 6);
  [[maybe_unused]] const MyQuantityMatrix quantities = values;
  return 0;
}
)",
                                                                                           "(no viable )?conversion from")}

TEST(TestQuantities2, ConstructExplicitlyFromQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyQuantity = LengthQuantityI;
  const MyQuantityMatrix quantities(MyQuantity(1), MyQuantity(2), MyQuantity(3), MyQuantity(4), MyQuantity(5), MyQuantity(6));
  const MyValueMatrix values(1, 2, 3, 4, 5, 6);
  EXPECT_EQ(quantities.getValues(), values);
}

TEST(TestQuantities2, ConstructExplicitlyFromValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyValueMatrix values(1, 2, 3, 4, 5, 6);
  const MyQuantityMatrix quantities(values);
  EXPECT_EQ(quantities.getValues(), values);
}

TEST(TestQuantities2, ConstructExplicitlyFromCsv) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyValueMatrix values(1, 2, 3, 4, 5, 6);
  const MyQuantityMatrix quantities(1, 2, 3, 4, 5, 6);
  EXPECT_EQ(quantities.getValues(), values);
}

TEST(TestQuantities2, ConstructExplicitlyFromConstValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  MyValueMatrix values(1, 2, 3, 4, 5, 6);
  const int v1 = 1;
  const int v2 = 2;
  const int v3 = 3;
  const int v4 = 4;
  const int v5 = 5;
  const int v6 = 6;
  const MyQuantityMatrix quantities(v1, v2, v3, v4, v5, v6);
  EXPECT_EQ(quantities.getValues(), values);
}

TEST(TestQuantities2, ConstructFromGoodQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  MyQuantityMatrix quantities(Quantity<ValueI, Length>(1), Quantity<ValueI, Area>(2), Quantity<ValueI, Volume>(3),
                              Quantity<ValueI, Mass>(4));
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
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<ValueT, Rows, Cols>;
#endif

using namespace wuzp;  // NOLINT

int main() {
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  MyQuantityMatrix quantities(QuantityI<Length>(1), QuantityI<Area>(2), QuantityI<Volume>(3), QuantityI<Length>(4));
  return 0;
}
  )",
                                               "Units do not match.")
}

class TestQuantitiesFixture : public ::testing::Test {
 public:
  using MyValueMatrix = ValueMatrixI<2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;

  const MyValueMatrix& getValues1() const { return values1_; }
  const MyValueMatrix& getValues2() const { return values2_; }
  const MyQuantityMatrix& getQuantities1() const { return quantities1_; }
  const MyQuantityMatrix& getQuantities2() const { return quantities2_; }
  MyQuantityMatrix& getQuantities1() { return quantities1_; }

 protected:
  void SetUp() override {
    quantities1_.setValues(values1_);
    quantities2_.setValues(values2_);
  }

 private:
  MyValueMatrix values1_{1, 2, 3, 4, 5, 6};
  MyQuantityMatrix quantities1_;
  MyValueMatrix values2_{3, 1, 4, 1, 5, 9};
  MyQuantityMatrix quantities2_;
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
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<ValueT, Rows, Cols>;
#endif

using namespace wuzp;  // NOLINT

int main() {
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length, Area, Volume, Mass, Force, Energy>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  MyQuantityMatrix quantities;
  quantities.set<1, 2>(QuantityI<Time>(7));
  return 0;
}
  )",
                                                                                            "Units do not match.")}

TEST_F(TestQuantitiesFixture, Sum) {  // NOLINT
  EXPECT_EQ(getQuantities1() + getQuantities2(), MyQuantityMatrix(getValues1() + getValues2()));
}

TEST_F(TestQuantitiesFixture, Difference) {  // NOLINT
  EXPECT_EQ(getQuantities1() - getQuantities2(), MyQuantityMatrix(getValues1() - getValues2()));
}

TEST_F(TestQuantitiesFixture, EqualityOperator) {  // NOLINT
  EXPECT_EQ(getQuantities1(), getQuantities1());
}

TEST_F(TestQuantitiesFixture, InequalityOperator) {  // NOLINT
  EXPECT_NE(getQuantities1(), getQuantities1() + getQuantities2());
}

TEST_F(TestQuantitiesFixture, Block) {  // NOLINT
  using MyValueMatrixB = ValueMatrix<int, 2, 2>;
  using MyUnitMatrixB = TypeMatrix<2, 2, Area, Volume, Force, Energy>;
  using MyQuantityMatrixB = Quantities<MyValueMatrixB, MyUnitMatrixB>;
  const MyValueMatrixB valuesB(2, 3, 5, 6);
  const MyQuantityMatrixB quantitiesB(valuesB);
  EXPECT_EQ((getQuantities1().block<0, 1, 2, 2>()), quantitiesB);
}

TEST_F(TestQuantitiesFixture, Transpose) {  // NOLINT
  using MyValueMatrixT = ValueMatrix<int, 3, 2>;
  using MyUnitMatrixT = TypeMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyQuantityMatrixT = Quantities<MyValueMatrixT, MyUnitMatrixT>;
  const MyValueMatrixT valuesT(1, 4, 2, 5, 3, 6);
  const MyQuantityMatrixT quantitiesT(valuesT);
  EXPECT_EQ(getQuantities1().transpose(), quantitiesT);
}

TEST(TestQuantities2, ProductQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  const MyValueMatrix values(1, 2, 3, 4);
  const MyQuantityMatrix quantities(values);
  EXPECT_EQ(quantities * quantities, MyQuantityMatrixResult(values * values));
}

TEST(TestQuantities2, ProductQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyQuantity = QuantityI<Length>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  const int value{2};
  const MyQuantity quantity(value);
  const MyValueMatrix values(1, 2, 3, 4);
  const MyQuantityMatrix quantities(values);
  EXPECT_EQ(quantities * quantity, MyQuantityMatrixResult(values * value));
  EXPECT_EQ(quantity * quantities, MyQuantityMatrixResult(values * value));
}

TEST(TestQuantities2, ProductValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix values(1, 2, 3, 4);
  const MyQuantityMatrix quantities(values);
  EXPECT_EQ(quantities * value, MyQuantityMatrix(values * value));
  EXPECT_EQ(value * quantities, MyQuantityMatrix(values * value));
}

TEST(TestQuantities2, DivisionQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyValue = int;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Dimensionless>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  const MyValue value{2};
  const LengthQuantityI quantity(value);
  const MyValueMatrix values(1, 2, 3, 4);
  const MyQuantityMatrix quantities(values);
  EXPECT_EQ(quantities / quantity, MyQuantityMatrixResult(values / value));
}

TEST(TestQuantities2, DivisionValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix values(1, 2, 3, 4);
  const MyQuantityMatrix quantities(values);
  EXPECT_EQ(quantities / value, MyQuantityMatrix(values / value));
}

TEST(TestQuantities2, Dot) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyQuantityResult = AreaQuantityI;
  const MyValueMatrix values1(1, 2, 3, 4);
  const MyValueMatrix values2(0, 3, -1, 2);
  EXPECT_EQ(MyQuantityMatrix(values1).dotProduct(MyQuantityMatrix(values2)), MyQuantityResult(11));
}

TEST(TestQuantities2, Chain) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix1 = TypeMatrix<2, 2, Length>;
  using MyUnitMatrix2 = TypeMatrix<2, 2, Area>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrix1 = Quantities<MyValueMatrix, MyUnitMatrix1>;
  using MyQuantityMatrix2 = Quantities<MyValueMatrix, MyUnitMatrix2>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  const MyValueMatrix values(1, 2, 3, 4);
  const MyQuantityMatrix1 quantities1(values);
  const MyQuantityMatrix2 quantities2(values);
  EXPECT_EQ(quantities2 + quantities1 * quantities1, MyQuantityMatrixResult(values + values * values));
}

class TestQuantitiesInvertible : public ::testing::Test {
 public:
  using MyValueMatrix = ValueMatrix<double, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Time>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;

  const MyValueMatrix& getValues() const { return values_; }
  const MyQuantityMatrix& getQuantities() const { return quantities_; }

 protected:
  void SetUp() override { quantities_.setValues(values_); }

 private:
  MyValueMatrix values_{4.0, 7.0, 2.0, 6.0};
  MyQuantityMatrix quantities_;
};

TEST_F(TestQuantitiesInvertible, Inverse) {  // NOLINT
  using MyUnitMatrixInv = TypeMatrix<2, 2, Frequency>;
  using MyQuantityMatrixInv = Quantities<MyValueMatrix, MyUnitMatrixInv>;
  const MyValueMatrix valuesInv{0.6, -0.7, -0.2, 0.4};
  const MyQuantityMatrixInv quantitiesInv(valuesInv);
  EXPECT_TRUE(getQuantities().inverse().isApprox(quantitiesInv));
}

TEST(TestQuantities, Norm) {  // NOLINT
  using MyValueVector = ValueVector<int, 3>;
  using MyUnitVector = TypeVector<3, Length>;
  using MyQuantityMatrix = Quantities<MyValueVector, MyUnitVector>;
  const MyQuantityMatrix quantities(-1, 2, 3);
  EXPECT_EQ(quantities.norm(), LengthQuantityI(quantities.getValues().getImplementation().norm()));
}

TEST(TestQuantities, SquaredNorm) {  // NOLINT
  using MyValueVector = ValueVector<int, 3>;
  using MyUnitVector = TypeVector<3, Length>;
  using MyQuantityMatrix = Quantities<MyValueVector, MyUnitVector>;
  const MyQuantityMatrix quantities(-1, 2, 3);
  EXPECT_EQ(quantities.squaredNorm(), SquaredLengthQuantityI(quantities.getValues().getImplementation().squaredNorm()));
}

TEST(TestQuantities, Determinant) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyQuantityMatrix quantities(-1, 2, 3, 1);
  EXPECT_EQ(quantities.determinant(), LengthQuantityI(quantities.getValues().getImplementation().determinant()));
}

#endif
