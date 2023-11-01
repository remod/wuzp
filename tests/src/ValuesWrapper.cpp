#include <gtest/gtest.h>

#include "wuzp/core/ValueWrapper.hpp"
#include "wuzp/core/traits.hpp"

#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

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

TEST(TestValuesWrapper, LazyProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 2, 2>;
  static_assert(!std::is_same<Product<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValuesWrapper, LazyDivision) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 2, 2>;
  static_assert(!std::is_same<Division<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}

#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestValuesWrapper, LazyInverse) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 2, 2>;
  static_assert(!std::is_same<Inverse<MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValuesWrapper, LazyTensorInverse) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 2, 2>;
  static_assert(!std::is_same<TensorInverse<MyValueMatrix>::type, MyValueMatrix>::value);
}
#endif

TEST(TestValuesWrapper, LazyContraction) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 2, 2>;
  static_assert(!std::is_same<Contraction<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValuesWrapper, DefaultConstructionDoesThrow) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  const MyValueMatrix matrix;
  EXPECT_NO_THROW((matrix.get<1, 1>()));
}

TEST(TestValuesWrapper, ConstructImplicityFromValuesWrapper) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  const MyValueMatrix matrix1;
  const MyValueMatrix matrix2;
  const MyValueMatrix matrix3 = matrix1 + matrix2;
  const MyValueMatrix matrix4 = matrix3;
  EXPECT_EQ(matrix4, matrix3);
}

TEST(TestValuesWrapper, ConstructExplicitlyFromValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  const MyValueMatrix values(1, 2, 3, 4, 5, 6);
  EXPECT_EQ((values.get<0, 0>()), 1);
}

TEST(TestValuesWrapper, ConstructExplicitlyFromValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 1, 1>;
  const MyValueMatrix values(1);
  EXPECT_EQ(values.get(), 1);
}

TEST(TestValuesWrapper, ConstructExplicitlyFromConstValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  const int v1 = 1;
  const int v2 = 2;
  const int v3 = 3;
  const int v4 = 4;
  const int v5 = 5;
  const int v6 = 6;
  MyValueMatrix values(v1, v2, v3, v4, v5, v6);
  EXPECT_EQ((values.get<0, 0>()), 1);
}

TEST(TestValuesWrapper, ConstructExplicitlyFromConstValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 1, 1>;
  const int v1 = 1;
  MyValueMatrix values(v1);
  EXPECT_EQ(values.get(), 1);
}

TEST(TestValuesWrapper, EqualityOperator) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 3> rhs(1, 2, 3, 4, 5, 6);
  EXPECT_EQ(lhs, rhs);
}

TEST(TestValuesWrapper, InequalityOperator) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 3> rhs(1, 2, 0, 4, 5, 6);
  EXPECT_NE(lhs, rhs);
}

TEST(TestValuesWrapper, Transpose) {  // NOLINT
  const ValueMatrix<int, 2, 3> matrix(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 3, 2> result(1, 4, 2, 5, 3, 6);
  EXPECT_EQ(matrix.transpose(), result);
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

TEST(TestValuesWrapper, ProductWithBuiltinValue) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const int rhs(3);
  const ValueMatrix<int, 2, 3> result(3, 6, 9, 12, 15, 18);
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(product(rhs, lhs), result);
  EXPECT_EQ(lhs * rhs, result);
  EXPECT_EQ(rhs * lhs, result);
}

TEST(TestValuesWrapper, ProductWithValue) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 1, 1> rhs(3);
  const ValueMatrix<int, 2, 3> result(3, 6, 9, 12, 15, 18);
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(product(rhs, lhs), result);
  EXPECT_EQ(lhs * rhs, result);
  EXPECT_EQ(rhs * lhs, result);
}

TEST(TestValuesWrapper, ProductWithValueOnLhsAndRhs) {  // NOLINT
  const ValueMatrix<int, 1, 1> lhs(3);
  const ValueMatrix<int, 1, 1> rhs(2);
  const ValueMatrix<int, 1, 1> result(6);
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(lhs * rhs, result);
}

TEST(TestValuesWrapper, ProductWithValues) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 3> rhs(3, 4, 1, 5, 6, 2);
  const ValueMatrix<int, 2, 3> result(3, 8, 3, 20, 30, 12);
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

// Blitz promotes two ints which are summed to a long.
// We could accept this as a result in this unit test, but unfortunately, the value matrix stores the value type itself too, which is still
// an int.
#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestValuesWrapper, ContractionTypeDerivation) {  // NOLINT
  using A = ValueMatrix<int, 4, 3>;
  using B = ValueMatrix<int, 3, 2>;
  static_assert(std::is_same_v<Evaluate<Contraction<A, B>::type>::type, ValueMatrix<int, 4, 2>>);
}
#endif

TEST(TestValuesWrapper, Contraction) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 3, 2> rhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 2> result(22, 28, 49, 64);
  EXPECT_EQ(contraction(lhs, rhs), result);
}

// TODO(remod): This test with Blitz currently results in a segfault.
#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
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
#endif

#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestValuesWrapper, CrossProduct) {  // NOLINT
  const ValueMatrix<int, 3, 1> lhs(1, 2, 3);
  const ValueMatrix<int, 3, 1> rhs(-1, 3, 4);
  const ValueMatrix<int, 3, 1> result(-1, -7, 5);
  EXPECT_EQ(lhs.crossProduct(rhs), result);
}
#endif

TEST(TestValuesWrapper, DivisionWithBuiltinValue) {  // NOLINT
  const ValueMatrix<float, 2, 3> lhs(1.f, 2.f, 3.f, 4.f, 5.f, 6.f);
  const float rhs(3.f);
  const ValueMatrix<float, 2, 3> result(1.f / 3.f, 2.f / 3.f, 1.f, 4.f / 3.f, 5.f / 3.f, 2.f);
  EXPECT_EQ(division(lhs, rhs), result);
  EXPECT_EQ(lhs / rhs, result);
}

TEST(TestValuesWrapper, DivisionWithValue) {  // NOLINT
  const ValueMatrix<float, 2, 3> lhs(1.f, 2.f, 3.f, 4.f, 5.f, 6.f);
  const ValueMatrix<float, 1, 1> rhs(3.f);
  const ValueMatrix<float, 2, 3> result(1.f / 3.f, 2.f / 3.f, 1.f, 4.f / 3.f, 5.f / 3.f, 2.f);
  EXPECT_EQ(division(lhs, rhs), result);
  EXPECT_EQ(lhs / rhs, result);
}

TEST(TestValuesWrapper, DivisionWithValueOnLhsAndRhs) {  // NOLINT
  const ValueMatrix<float, 1, 1> lhs(1.f);
  const ValueMatrix<float, 1, 1> rhs(3.f);
  const ValueMatrix<float, 1, 1> result(1.f / 3.f);
  EXPECT_EQ(division(lhs, rhs), result);
  EXPECT_EQ(lhs / rhs, result);
}

TEST(TestValuesWrapper, Division) {  // NOLINT
  const ValueMatrix<float, 2, 3> lhs(1.f, 2.f, 3.f, 4.f, 5.f, 6.f);
  const ValueMatrix<float, 2, 3> rhs(3.f, 4.f, 1.f, 5.f, 6.f, 2.f);
  const ValueMatrix<float, 2, 3> result(1.f / 3.f, 2.f / 4.f, 3.f, 4.f / 5.f, 5.f / 6.f, 3.f);
  EXPECT_EQ(division(lhs, rhs), result);
  EXPECT_EQ(lhs / rhs, result);
}

#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestValuesWrapper, Inverse) {  // NOLINT
  const ValueMatrix<double, 3, 2> matrix(1.0, 2.0, 4.0, 0.5, 10.0, 0.2);
  const ValueMatrix<double, 3, 2> result(1.0, 0.5, 0.25, 2.0, 0.1, 5.0);
  EXPECT_TRUE(matrix.inverse().isApprox(result));
}

TEST(TestValuesWrapper, TensorInverse) {  // NOLINT
  const ValueMatrix<double, 2, 2> matrix(4.0, 7.0, 2.0, 6.0);
  const ValueMatrix<double, 2, 2> result(0.6, -0.7, -0.2, 0.4);
  EXPECT_TRUE(matrix.tensorInverse().isApprox(result));
}
#endif

TEST(TestValuesWrapper, InitializeMatrix) {  // NOLINT
  const ValueMatrix<int, 3, 2> matrix(1, 2, 3, 4, 5, 6);
  EXPECT_EQ((matrix.get<1, 0>()), 3);
  EXPECT_EQ(matrix.get(1, 0), 3);
}

TEST(TestValuesWrapper, InitializeValue) {  // NOLINT
  const Value<int> value(1);
  EXPECT_EQ(value.get(), 1);
}

TEST(TestValuesWrapper, Evaluate) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs(1, 2, 3, 4, 5, 6);
  const ValueMatrix<int, 2, 3> rhs(3, 4, 1, 5, 6, 2);
  const ValueMatrix<int, 2, 3> result(4, 6, 4, 9, 11, 8);
  EXPECT_EQ(sum(lhs, rhs).evaluate(), result);
}

// TODO(remod): Compile-time check number of constructor arguments and test it.
