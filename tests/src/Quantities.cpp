#include <gtest/gtest.h>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

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

TEST(TestQuantities2, ConstructImplicitlyFromQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyQuantityMatrix quantities1{1, 2, 3, 4, 5, 6};
  const MyQuantityMatrix quantities2{6, 5, 4, 3, 2, 1};
  // Use = instead of {} to trigger implicit conversion.
  const MyQuantityMatrix quantities3 = quantities1 + quantities2;
  const MyQuantityMatrix quantities = quantities3;
  EXPECT_EQ(quantities, quantities3);
  EXPECT_EQ(quantities, (MyQuantityMatrix{7, 7, 7, 7, 7, 7}));
}

TEST(TestQuantities2, ConstructExplicitlyFromQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyQuantity = LengthQuantityI;
  const MyQuantityMatrix quantities{MyQuantity{1}, MyQuantity{2}, MyQuantity{3}, MyQuantity{4}, MyQuantity{5}, MyQuantity{6}};
  const MyValueMatrix values{1, 2, 3, 4, 5, 6};
  EXPECT_EQ(quantities.getValues(), values);
}

TEST(TestQuantities2, ConstructExplicitlyFromValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyValueMatrix values{1, 2, 3, 4, 5, 6};
  const MyQuantityMatrix quantities{values};
  EXPECT_EQ(quantities.getValues(), values);
}

TEST(TestQuantities2, ConstructExplicitlyFromCsv) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyValueMatrix values{1, 2, 3, 4, 5, 6};
  const MyQuantityMatrix quantities{1, 2, 3, 4, 5, 6};
  EXPECT_EQ(quantities.getValues(), values);
}

TEST(TestQuantities2, ConstructExplicitlyFromConstValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  using MyUnitMatrix = TypeMatrix<2, 3, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyValueMatrix values{1, 2, 3, 4, 5, 6};
  const int v1{1};
  const int v2{2};
  const int v3{3};
  const int v4{4};
  const int v5{5};
  const int v6{6};
  const MyQuantityMatrix quantities{v1, v2, v3, v4, v5, v6};
  EXPECT_EQ(quantities.getValues(), values);
}

TEST(TestQuantities2, ConstructFromGoodQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyQuantityMatrix quantities{Quantity<ValueI, Length>{1}, Quantity<ValueI, Area>{2}, Quantity<ValueI, Volume>{3},
                                    Quantity<ValueI, Mass>{4}};
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
  EXPECT_EQ(getValues1(), getValues1());
  std::cerr << getQuantities1().getValues() << std::endl;

  EXPECT_EQ(getQuantities1().getValues(), getValues1());
  getQuantities1().setValues(getValues2());
  EXPECT_EQ(getQuantities1().getValues(), getValues2());
}

TEST_F(TestQuantitiesFixture, SetGetQuantity) {  // NOLINT
  using EnergyQuantity = QuantityI<Energy>;
  EXPECT_EQ((getQuantities1().get<1, 2>()), (EnergyQuantity{getValues1().get<1, 2>()}));
  const EnergyQuantity quantity{7};
  getQuantities1().set<1, 2>(quantity);
  EXPECT_EQ((getQuantities1().get<1, 2>()), quantity);
}

TEST_F(TestQuantitiesFixture, Sum) {  // NOLINT
  EXPECT_EQ(getQuantities1() + getQuantities2(), (MyQuantityMatrix{4, 3, 7, 5, 10, 15}));
}

TEST_F(TestQuantitiesFixture, Difference) {  // NOLINT
  EXPECT_EQ(getQuantities1() - getQuantities2(), (MyQuantityMatrix{-2, 1, -1, 3, 0, -3}));
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
  const MyValueMatrixB valuesB{2, 3, 5, 6};
  const MyQuantityMatrixB quantitiesB{valuesB};
  EXPECT_EQ((getQuantities1().block<0, 1, 2, 2>()), quantitiesB);
}

TEST_F(TestQuantitiesFixture, Transpose) {  // NOLINT
  using MyValueMatrixT = ValueMatrix<int, 3, 2>;
  using MyUnitMatrixT = TypeMatrix<3, 2, Length, Mass, Area, Force, Volume, Energy>;
  using MyQuantityMatrixT = Quantities<MyValueMatrixT, MyUnitMatrixT>;
  const MyValueMatrixT valuesT{1, 4, 2, 5, 3, 6};
  const MyQuantityMatrixT quantitiesT{valuesT};
  EXPECT_EQ(getQuantities1().transpose(), quantitiesT);
}

TEST(TestQuantities2, ProductWithBuiltinValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyQuantityMatrix quantities{values};
  const MyQuantityMatrix result{2, 4, 6, 8};
  EXPECT_EQ(product(quantities, value), result);
  EXPECT_EQ(product(value, quantities), result);
  EXPECT_EQ(quantities * value, result);
  EXPECT_EQ(value * quantities, result);
}

TEST(TestQuantities2, ProductWithBuiltinValueChained) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 1, 1>;
  using MyUnitMatrix = TypeMatrix<1, 1, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyUnitMatrixResult = TypeMatrix<1, 1, Area>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix values{1};
  const MyQuantityMatrix quantities{values};
  const MyQuantityMatrixResult result{2};
  EXPECT_EQ(value * (quantities * quantities), result);
}

TEST(TestQuantities2, ProductWithValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const ValueI value{2};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyQuantityMatrix quantities{values};
  const MyQuantityMatrix result{2, 4, 6, 8};
  EXPECT_EQ(product(quantities, value), result);
  EXPECT_EQ(product(value, quantities), result);
  EXPECT_EQ(quantities * value, result);
  EXPECT_EQ(value * quantities, result);
}

TEST(TestQuantities2, ProductWithValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyValueMatrix values{1, 2, 3, 4};
  const MyQuantityMatrix quantities{values};
  const MyQuantityMatrix result{1, 4, 9, 16};
  EXPECT_EQ(product(quantities, values), result);
  EXPECT_EQ(product(values, quantities), result);
  EXPECT_EQ(quantities * values, result);
  EXPECT_EQ(values * quantities, result);
}

TEST(TestQuantities2, ProductWithQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyQuantity = QuantityI<Length>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  const int value{2};
  const MyQuantity quantity{value};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyQuantityMatrix quantities{values};
  const MyQuantityMatrixResult result{2, 4, 6, 8};
  EXPECT_EQ(product(quantities, quantity), result);
  EXPECT_EQ(product(quantity, quantities), result);
  EXPECT_EQ(quantities * quantity, result);
  EXPECT_EQ(quantity * quantities, result);
}

TEST(TestQuantities2, ProductWithQuantities) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  const MyValueMatrix values{1, 2, 3, 4};
  const MyQuantityMatrix quantities{values};
  const MyQuantityMatrixResult result{1, 4, 9, 16};
  EXPECT_EQ(product(quantities, quantities), result);
  EXPECT_EQ(quantities * quantities, result);
}

// TODO(remod): This test with Blitz currently results in a segfault.
#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestQuantities2, DotProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyQuantityMatrix lhs{1, 2, 3, 4};
  const MyQuantityMatrix rhs{0, 3, -1, 2};
  const AreaQuantityI result{11};
  EXPECT_EQ(lhs.dotProduct(rhs), result);
}

TEST(TestQuantities2, DotProduct1x1Matrices) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 1, 1>;
  using MyUnitMatrix = TypeMatrix<1, 1, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyQuantityMatrix lhs{1};
  const MyQuantityMatrix rhs{2};
  const AreaQuantityI result{2};
  EXPECT_EQ(lhs.dotProduct(rhs), result);
}
#endif

#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestQuantities2, CrossProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 3, 1>;
  using MyUnitMatrix = TypeMatrix<3, 1, Length>;
  using MyUnitMatrixResult = TypeMatrix<3, 1, Area>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  const MyQuantityMatrix lhs{1, 2, 3};
  const MyQuantityMatrix rhs{-1, 3, 4};
  const MyQuantityMatrixResult result{-1, -7, 5};
  EXPECT_EQ(lhs.crossProduct(rhs), result);
}
#endif

TEST(TestQuantities2, DivisionWithBuiltinValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyValue = int;
  const MyValue value{2};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyQuantityMatrix quantities{values};
  EXPECT_EQ(division(quantities, value), MyQuantityMatrix{values / value});
  EXPECT_EQ(quantities / value, MyQuantityMatrix{values / value});
}

TEST(TestQuantities2, DivisionWithValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const ValueI value{2};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyQuantityMatrix quantities{values};
  EXPECT_EQ(division(quantities, value), MyQuantityMatrix{values / value});
  EXPECT_EQ(quantities / value, MyQuantityMatrix{values / value});
}

TEST(TestQuantities2, DivisionWithQuantity) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyValue = int;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Dimensionless>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  const MyValue value{2};
  const LengthQuantityI quantity{value};
  const MyValueMatrix values{1, 2, 3, 4};
  const MyQuantityMatrix quantities{values};
  EXPECT_EQ(division(quantities, quantity), (MyQuantityMatrixResult{0, 1, 1, 2}));
  EXPECT_EQ(quantities / quantity, (MyQuantityMatrixResult{0, 1, 1, 2}));
}

TEST(TestQuantities2, Chain) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix1 = TypeMatrix<2, 2, Length>;
  using MyUnitMatrix2 = TypeMatrix<2, 2, Area>;
  using MyUnitMatrixResult = TypeMatrix<2, 2, Area>;
  using MyQuantityMatrix1 = Quantities<MyValueMatrix, MyUnitMatrix1>;
  using MyQuantityMatrix2 = Quantities<MyValueMatrix, MyUnitMatrix2>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  const MyValueMatrix values{1, 2, 3, 4};
  const MyQuantityMatrix1 quantities1{values};
  const MyQuantityMatrix2 quantities2{values};
  EXPECT_EQ(quantities2 + quantities1 * quantities1, (MyQuantityMatrixResult{2, 6, 12, 20}));
}

#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestQuantities, Inverse) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  using MyUnitMatrix = TypeMatrix<3, 2, Time>;
  using MyUnitMatrixResult = TypeMatrix<3, 2, Frequency>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using MyQuantityMatrixResult = Quantities<MyValueMatrix, MyUnitMatrixResult>;
  const MyQuantityMatrix quantities{1.0, 2.0, 4.0, 0.5, 10.0, 0.2};
  const MyQuantityMatrixResult result{1.0, 0.5, 0.25, 2.0, 0.1, 5.0};
  EXPECT_TRUE(quantities.inverse().isApprox(result));
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

TEST_F(TestQuantitiesInvertible, TensorInverse) {  // NOLINT
  using MyUnitMatrixInv = TypeMatrix<2, 2, Frequency>;
  using MyQuantityMatrixInv = Quantities<MyValueMatrix, MyUnitMatrixInv>;
  const MyValueMatrix valuesInv{0.6, -0.7, -0.2, 0.4};
  const MyQuantityMatrixInv quantitiesInv{valuesInv};
  EXPECT_TRUE(getQuantities().tensorInverse().isApprox(quantitiesInv));
}

TEST(TestQuantities, Norm) {  // NOLINT
  using MyValueVector = ValueVector<int, 3>;
  using MyUnitVector = TypeVector<3, Length>;
  using MyQuantityMatrix = Quantities<MyValueVector, MyUnitVector>;
  const MyQuantityMatrix quantities{3, 0, 4};
  EXPECT_EQ(quantities.norm(), LengthQuantityI{5});
}

TEST(TestQuantities, SquaredNorm) {  // NOLINT
  using MyValueVector = ValueVector<int, 3>;
  using MyUnitVector = TypeVector<3, Length>;
  using MyQuantityMatrix = Quantities<MyValueVector, MyUnitVector>;
  const MyQuantityMatrix quantities{3, 0, 4};
  EXPECT_EQ(quantities.squaredNorm(), SquaredLengthQuantityI{25});
}

TEST(TestQuantities, Determinant) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Length>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  const MyQuantityMatrix quantities{-1, 2, 3, 1};
  EXPECT_EQ(quantities.determinant(), AreaQuantityI{-7});
}

TEST(TestQuantities, DeterminantOfMassMatrix) {  // NOLINT
  // A mass matrix with a translational and a rotational degree of freedom: [[kg, kg m], [kg m, kg m^2]]. Its units describe
  // a linear map (from accelerations to forces and torques), so its determinant has a well-defined unit: kg^2 m^2.
  using MassLength = Product<Mass, Length>::type;
  using MassArea = Product<Mass, Area>::type;
  using MyValueMatrix = ValueMatrix<int, 2, 2>;
  using MyUnitMatrix = TypeMatrix<2, 2, Mass, MassLength, MassLength, MassArea>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  using DeterminantQuantity = QuantityI<Product<Power<Mass, Integer<2>>::type, Area>::type>;
  const MyQuantityMatrix quantities{2, 3, 3, 5};
  EXPECT_EQ(quantities.determinant(), DeterminantQuantity{1});
}
#endif
