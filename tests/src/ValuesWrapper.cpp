#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"
#include "wuzp/core/ValueWrapper.hpp"

using namespace wuzp;  // NOLINT

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
using namespace wuzp_eigen;  // NOLINT
#elif VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_BLITZ
#include "wuzp_blitz/ValueMatrix.hpp"
using namespace wuzp_blitz;  // NOLINT
#endif

TEST(TestValuesWrapper, Dimension) {  // NOLINT
  static_assert(Dimension<ValueMatrix<double, 1, 1>>::value == 0);
  static_assert(Dimension<ValueMatrix<double, 3, 1>>::value == 1);
  static_assert(Dimension<ValueMatrix<double, 1, 2>>::value == 1);
  static_assert(Dimension<ValueMatrix<double, 3, 2>>::value == 2);
}

TEST(TestValuesWrapper, Rows) {  // NOLINT
  static_assert(Rows<ValueMatrix<double, 3, 2>>::value == 3);
}

TEST(TestValuesWrapper, Cols) {  // NOLINT
  static_assert(Cols<ValueMatrix<double, 3, 2>>::value == 2);
}

TEST(TestValuesWrapper, LazyBlock) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<Block<MyValueMatrix, 0, 0, 1, 1>::type, MyValueMatrix>::value);
}

TEST(TestValuesWrapper, LazyTranspose) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<Transpose<MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValuesWrapper, LazySum) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<Sum<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValuesWrapper, LazyDifference) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<Difference<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValuesWrapper, LazyContraction) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 2, 2>;
  static_assert(!std::is_same<Contraction<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValuesWrapper, EqualityOperator) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 3> rhs(1, 2, 3, 4, 5, 6);
  EXPECT_TRUE(lhs == rhs);
}

TEST(TestValuesWrapper, Sum) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 3> rhs(3, 4, 1, 5, 6, 2);
  const ValueMatrix<int, 2, 3> result(4, 6, 4, 9, 11, 8);
  EXPECT_EQ(sum(lhs, rhs), result);
  EXPECT_EQ(lhs + rhs, result);
}

// We do _not_ allow matrix + scalar operations as this could indicate a mistake.

TEST(TestValuesWrapper, Difference) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 3> rhs(3, 4, 1, 5, 6, 2);
  const ValueMatrix<int, 2, 3> result(-2, -2, 2, -1, -1, 4);
  EXPECT_EQ(difference(lhs, rhs), result);
  EXPECT_EQ(lhs - rhs, result);
}

// We do _not_ allow matrix - scalar operations as this could indicate a mistake.

TEST(TestValuesWrapper, Negation) {  // NOLINT
  const ValueMatrix<int, 2, 3> rhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 3> result(-1, -2, -3, -4, -5, -6);
  EXPECT_EQ(negation(rhs), result);
  EXPECT_EQ(-rhs, result);
}

TEST(TestValuesWrapper, Product) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 3> rhs(3, 4, 1, 5, 6, 2);
  const ValueMatrix<int, 2, 3> result(3, 8, 3, 20, 30, 12);
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(lhs * rhs, result);
}

TEST(TestValuesWrapper, ProductWith1x1MatrixOnRhs) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 1, 1> rhs(3);
  const ValueMatrix<int, 2, 3> result(3, 6, 9, 12, 15, 18);
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(lhs * rhs, result);
}

TEST(TestValuesWrapper, ProductWith1x1MatrixOnLhs) {  // NOLINT
  const ValueMatrix<int, 1, 1> lhs(3);
  const ValueMatrix<int, 2, 3> rhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 3> result(3, 6, 9, 12, 15, 18);
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(lhs * rhs, result);
}

TEST(TestValuesWrapper, ProductWith1x1MatricesOnLhsAndRhs) {  // NOLINT
  const ValueMatrix<int, 1, 1> lhs(3);
  const ValueMatrix<int, 1, 1> rhs(2);
  const ValueMatrix<int, 1, 1> result(6);
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(lhs * rhs, result);
}

TEST(TestValuesWrapper, ProductWithValueOnRhs) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const int rhs(3);
  const ValueMatrix<int, 2, 3> result(3, 6, 9, 12, 15, 18);
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(lhs * rhs, result);
}

TEST(TestValuesWrapper, ProductWithValueOnLhs) {  // NOLINT
  const int lhs(3);
  const ValueMatrix<int, 2, 3> rhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 3> result(3, 6, 9, 12, 15, 18);
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(lhs * rhs, result);
}

TEST(TestValuesWrapper, TensorProduct) {  // NOLINT
    // TODO(remod): Implement
//  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
//  const ValueMatrix<int, 2, 3> rhs(3, 4, 1, 5, 6, 2);
//  const ValueMatrix<int, 2, 3> result(3, 8, 3, 20, 30, 12);
//  EXPECT_EQ(tensorProduct(lhs, rhs), result);
}

TEST(TestValuesWrapper, DotProduct) {  // NOLINT
  const ValueMatrix<int, 4, 1> lhs(1, 2, 3, 4);
  const ValueMatrix<int, 4, 1> rhs(0, 3, -1, 2);
  const ValueWrapper<int> result(11);
  EXPECT_EQ(lhs.dotProduct(rhs), result);
}

TEST(TestValuesWrapper, DotProduct1x1Matrices) {  // NOLINT
  const ValueMatrix<int, 1, 1> lhs(1);
  const ValueMatrix<int, 1, 1> rhs(2);
  const ValueWrapper<int> result(2);
  EXPECT_EQ(lhs.dotProduct(rhs), result);
}

TEST(TestValuesWrapper, CrossProduct) {  // NOLINT
  const ValueMatrix<int, 3, 1> lhs(1, 2, 3);
  const ValueMatrix<int, 3, 1> rhs(-1, 3, 4);
  const ValueMatrix<int, 3, 1> result(-1, -7, 5);
  EXPECT_EQ(lhs.crossProduct(rhs), result);
}

TEST(TestValuesWrapper, Contraction) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 3, 2> rhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 2> result(1, 2, 3, 4);
  EXPECT_EQ(contraction(lhs, rhs), result);
}

TEST(TestValuesWrapper, Division) {  // NOLINT
  const ValueMatrix<float, 2, 3> lhs(1.f, 2.f, 3.f, 4.f, 5.f, 6.f);
  const ValueMatrix<float, 2, 3> rhs(3.f, 4.f, 1.f, 5.f, 6.f, 2.f);
  const ValueMatrix<float, 2, 3> result(1.f / 3.f, 2.f / 4.f, 3.f, 4.f / 5.f, 5.f / 6.f, 3.f);
  EXPECT_EQ(division(lhs, rhs), result);
  EXPECT_EQ(lhs / rhs, result);
}

TEST(TestValuesWrapper, DivisionWith1x1Matrix) {  // NOLINT
  const ValueMatrix<float, 2, 3> lhs(1.f, 2.f, 3.f, 4.f, 5.f, 6.f);
  const ValueMatrix<float, 1, 1> rhs(3.f);
  const ValueMatrix<float, 2, 3> result(1.f / 3.f, 2.f / 3.f, 1.f, 4.f / 3.f, 5.f / 3.f, 2.f);
  EXPECT_EQ(division(lhs, rhs), result);
  EXPECT_EQ(lhs / rhs, result);
}

TEST(TestValuesWrapper, DivisionWith1x1MatricesOnLhsAndRhs) {  // NOLINT
  const ValueMatrix<float, 1, 1> lhs(1.f);
  const ValueMatrix<float, 1, 1> rhs(3.f);
  const ValueMatrix<float, 1, 1> result(1.f / 3.f);
  EXPECT_EQ(division(lhs, rhs), result);
  EXPECT_EQ(lhs / rhs, result);
}

TEST(TestValuesWrapper, DivisionWithValue) {  // NOLINT
  const ValueMatrix<float, 2, 3> lhs(1.f, 2.f, 3.f, 4.f, 5.f, 6.f);
  const float rhs(3.f);
  const ValueMatrix<float, 2, 3> result(1.f / 3.f, 2.f / 3.f, 1.f, 4.f / 3.f, 5.f / 3.f, 2.f);
  EXPECT_EQ(division(lhs, rhs), result);
  EXPECT_EQ(lhs / rhs, result);
}

TEST(TestValuesWrapper, InitializeMatrix) {  // NOLINT
  const ValueMatrix<int, 3, 2> matrix(1, 2, 3, 4, 5, 6);
  EXPECT_EQ((matrix.get<1, 0>()), 3);
  EXPECT_EQ(matrix.get(1, 0), 3);
}

TEST(TestValuesWrapper, InitializeValue) {  // NOLINT
  const Value<int> value(1);
  EXPECT_EQ(value.get(), 1);
}

// TODO(remod): Compile-time check number of constructor arguments and test it.
