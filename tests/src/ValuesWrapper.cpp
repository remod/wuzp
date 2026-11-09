#include <cstdlib>
#include <type_traits>
#include <utility>

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

// The Blitz++ backend implements neither inverses nor determinants.
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

// The Blitz++ backend evaluates inner products: copying Blitz++'s lazy reduction reads an uninitialized member (#59).
#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestValuesWrapper, LazyInnerProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 2, 2>;
  static_assert(!std::is_same<InnerProduct<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}
#else
TEST(TestValuesWrapper, EvaluatedInnerProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 2, 2>;
  static_assert(std::is_same<InnerProduct<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}
#endif

TEST(TestValuesWrapper, DefaultConstructionDoesThrow) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  const MyValueMatrix matrix;
  EXPECT_NO_THROW((matrix.get<1, 1>()));
}

TEST(TestValuesWrapper, ConstructImplicityFromValuesWrapper) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  const MyValueMatrix matrix1{1, 2, 3, 4, 5, 6};
  const MyValueMatrix matrix2{6, 5, 4, 3, 2, 1};
  // Use = instead of {} to trigger implicit conversion.
  const MyValueMatrix matrix3 = matrix1 + matrix2;
  const MyValueMatrix matrix4 = matrix3;
  EXPECT_EQ(matrix4, matrix3);
  EXPECT_EQ(matrix4, (MyValueMatrix{7, 7, 7, 7, 7, 7}));
}

TEST(TestValuesWrapper, ConstructExplicitlyFromValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  const MyValueMatrix values{1, 2, 3, 4, 5, 6};
  EXPECT_EQ((values.get<0, 0>()), 1);
}

TEST(TestValuesWrapper, ConstructExplicitlyFromValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 1, 1>;
  const MyValueMatrix values{1};
  EXPECT_EQ(values.get(), 1);
}

TEST(TestValuesWrapper, ConstructExplicitlyFromConstValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  const int v1{1};
  const int v2{2};
  const int v3{3};
  const int v4{4};
  const int v5{5};
  const int v6{6};
  const MyValueMatrix values{v1, v2, v3, v4, v5, v6};
  EXPECT_EQ((values.get<0, 0>()), 1);
}

TEST(TestValuesWrapper, ConstructExplicitlyFromConstValue) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 1, 1>;
  const int v1{1};
  const MyValueMatrix values{v1};
  EXPECT_EQ(values.get(), 1);
}

TEST(TestValuesWrapper, EqualityOperator) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs{1, 2, 3, 4, 5, 6};
  const ValueMatrix<int, 2, 3> rhs{1, 2, 3, 4, 5, 6};
  EXPECT_EQ(lhs, rhs);
}

TEST(TestValuesWrapper, InequalityOperator) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs{1, 2, 3, 4, 5, 6};
  const ValueMatrix<int, 2, 3> rhs{1, 2, 0, 4, 5, 6};
  EXPECT_NE(lhs, rhs);
}

TEST(TestValuesWrapper, IsApprox) {  // NOLINT
  const ValueMatrix<double, 2, 3> matrix{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  const ValueMatrix<double, 2, 3> approx{1.0, 2.0, 3.0, 4.0, 5.0, 6.0 + 1e-13};
  const ValueMatrix<double, 2, 3> notApprox{1.0, 2.0, 3.0, 4.0, 5.0, 6.0 + 1e-11};
  EXPECT_TRUE(matrix.isApprox(approx));
  EXPECT_FALSE(matrix.isApprox(notApprox));
}

TEST(TestValuesWrapper, IsApproxDefaultPrecisionOfFloats) {  // NOLINT
  // Floats have a default precision of 1e-5, relative to the magnitude of the values (here 5).
  const ValueMatrix<float, 2, 1> matrix{3.0f, 4.0f};
  const ValueMatrix<float, 2, 1> approx{3.0f, 4.0f + 1e-5f};
  const ValueMatrix<float, 2, 1> notApprox{3.0f, 4.0f + 1e-3f};
  EXPECT_TRUE(matrix.isApprox(approx));
  EXPECT_FALSE(matrix.isApprox(notApprox));
}

TEST(TestValuesWrapper, IsApproxDefaultPrecisionOfIntegers) {  // NOLINT
  // Integers have a default precision of 0: only equal values are approximately equal.
  const ValueMatrix<int, 2, 1> matrix{3, 4};
  const ValueMatrix<int, 2, 1> notApprox{3, 5};
  EXPECT_TRUE(matrix.isApprox(matrix));
  EXPECT_FALSE(matrix.isApprox(notApprox));
}

TEST(TestValuesWrapper, Zero) {  // NOLINT
  const ValueMatrix<double, 2, 3> expected{0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  EXPECT_EQ((ValueMatrix<double, 2, 3>::zero()), expected);
  const ValueMatrix<int, 2, 1> expectedIntegers{0, 0};
  EXPECT_EQ((ValueMatrix<int, 2, 1>::zero()), expectedIntegers);
}

TEST(TestValuesWrapper, Random) {  // NOLINT
  // The values are uniformly distributed in [-1, 1]: with many of them, all are in the range and they are not all equal.
  // The backends use std::rand, so a fixed seed makes the test repeatable.
  std::srand(1);
  const auto values = ValueMatrix<double, 10, 10>::random();
  for (int row = 0; row < 10; ++row) {
    for (int col = 0; col < 10; ++col) {
      EXPECT_GE(values.get(row, col), -1.0);
      EXPECT_LE(values.get(row, col), 1.0);
    }
  }
  EXPECT_NE(values.get(0, 0), values.get(9, 9));
}

TEST(TestValuesWrapper, IsApproxIsRelative) {  // NOLINT
  // The precision is relative to the magnitude of the values, with every backend.
  const ValueMatrix<double, 2, 1> large{1.0e6, 2.0e6};
  const ValueMatrix<double, 2, 1> largeApprox{1.0e6 + 1.0, 2.0e6};
  EXPECT_TRUE(large.isApprox(largeApprox, 1.0e-5));
  const ValueMatrix<double, 2, 1> small{1.0e-6, 2.0e-6};
  const ValueMatrix<double, 2, 1> smallNotApprox{2.0e-6, 2.0e-6};
  EXPECT_FALSE(small.isApprox(smallNotApprox, 1.0e-3));
}

TEST(TestValuesWrapper, Transpose) {  // NOLINT
  const ValueMatrix<int, 2, 3> matrix{1, 2, 3, 4, 5, 6};
  const ValueMatrix<int, 3, 2> result{1, 4, 2, 5, 3, 6};
  EXPECT_EQ(matrix.transpose(), result);
}

TEST(TestValuesWrapper, Sum) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs{1, 2, 3, 4, 5, 6};
  const ValueMatrix<int, 2, 3> rhs{3, 4, 1, 5, 6, 2};
  const ValueMatrix<int, 2, 3> result{4, 6, 4, 9, 11, 8};
  EXPECT_EQ(sum(lhs, rhs), result);
  EXPECT_EQ(lhs + rhs, result);
}

// We do _not_ allow matrix + scalar operations as this could indicate a mistake.

TEST(TestValuesWrapper, Difference) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs{1, 2, 3, 4, 5, 6};
  const ValueMatrix<int, 2, 3> rhs{3, 4, 1, 5, 6, 2};
  const ValueMatrix<int, 2, 3> result{-2, -2, 2, -1, -1, 4};
  EXPECT_EQ(difference(lhs, rhs), result);
  EXPECT_EQ(lhs - rhs, result);
}

// We do _not_ allow matrix - scalar operations as this could indicate a mistake.

TEST(TestValuesWrapper, Negation) {  // NOLINT
  const ValueMatrix<int, 2, 3> rhs{1, 2, 3, 4, 5, 6};
  const ValueMatrix<int, 2, 3> result{-1, -2, -3, -4, -5, -6};
  EXPECT_EQ(negation(rhs), result);
  EXPECT_EQ(-rhs, result);
}

TEST(TestValuesWrapper, ProductWithBuiltinValue) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs{1, 2, 3, 4, 5, 6};
  const int rhs{3};
  const ValueMatrix<int, 2, 3> result{3, 6, 9, 12, 15, 18};
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(product(rhs, lhs), result);
  EXPECT_EQ(lhs * rhs, result);
  EXPECT_EQ(rhs * lhs, result);
}

TEST(TestValuesWrapper, ProductWithValue) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs{1, 2, 3, 4, 5, 6};
  const ValueMatrix<int, 1, 1> rhs{3};
  const ValueMatrix<int, 2, 3> result{3, 6, 9, 12, 15, 18};
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(product(rhs, lhs), result);
  EXPECT_EQ(lhs * rhs, result);
  EXPECT_EQ(rhs * lhs, result);
}

TEST(TestValuesWrapper, ProductWithValueOnLhsAndRhs) {  // NOLINT
  const ValueMatrix<int, 1, 1> lhs{3};
  const ValueMatrix<int, 1, 1> rhs{2};
  const ValueMatrix<int, 1, 1> result{6};
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(lhs * rhs, result);
}

TEST(TestValuesWrapper, ProductWithValues) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs{1, 2, 3, 4, 5, 6};
  const ValueMatrix<int, 2, 3> rhs{3, 4, 1, 5, 6, 2};
  const ValueMatrix<int, 2, 3> result{3, 8, 3, 20, 30, 12};
  EXPECT_EQ(product(lhs, rhs), result);
  EXPECT_EQ(lhs * rhs, result);
}

TEST(TestValuesWrapper, TensorProduct) {  // NOLINT
                                          // TODO(remod): Implement
  //  const ValueMatrix<int, 2, 3> lhs{1, 2, 3, 4, 5, 6};
  //  const ValueMatrix<int, 2, 3> rhs{3, 4, 1, 5, 6, 2};
  //  const ValueMatrix<int, 2, 3> result{3, 8, 3, 20, 30, 12};
  //  EXPECT_EQ(tensorProduct(lhs, rhs), result);
}

// The inner product keeps the value type, also with Blitz++, which widens sums (e.g. of int to long, and of float to double).
TEST(TestValuesWrapper, InnerProductTypeDerivation) {  // NOLINT
  using A = ValueMatrix<int, 4, 3>;
  using B = ValueMatrix<int, 3, 2>;
  static_assert(std::is_same_v<Evaluate<InnerProduct<A, B>::type>::type, ValueMatrix<int, 4, 2>>);
  using AF = ValueMatrix<float, 4, 3>;
  using BF = ValueMatrix<float, 3, 2>;
  static_assert(std::is_same_v<Evaluate<InnerProduct<AF, BF>::type>::type, ValueMatrix<float, 4, 2>>);
}

TEST(TestValuesWrapper, InnerProduct) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs{1, 2, 3, 4, 5, 6};
  const ValueMatrix<int, 3, 2> rhs{1, 2, 3, 4, 5, 6};
  const ValueMatrix<int, 2, 2> result{22, 28, 49, 64};
  EXPECT_EQ(innerProduct(lhs, rhs), result);
}

TEST(TestValuesWrapper, InnerProductOfFloats) {  // NOLINT
  const ValueMatrix<float, 2, 3> lhs{1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
  const ValueMatrix<float, 3, 2> rhs{1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
  const ValueMatrix<float, 2, 2> result{22.f, 28.f, 49.f, 64.f};
  EXPECT_TRUE(innerProduct(lhs, rhs).isApprox(result, 1e-6f));
}

TEST(TestValuesWrapper, OuterProductOfScalars) {  // NOLINT
  const ValueMatrix<int, 1, 1> lhs{2};
  const ValueMatrix<int, 1, 1> rhs{3};
  const ValueMatrix<int, 1, 1> result{6};
  EXPECT_EQ(outerProduct(lhs, rhs), result);
}

TEST(TestValuesWrapper, OuterProductOfCovectorAndVector) {  // NOLINT
  const ValueMatrix<int, 1, 2> lhs{1, 2};
  const ValueMatrix<int, 3, 1> rhs{-1, 3, 4};
  const ValueMatrix<int, 3, 2> result{-1, -2, 3, 6, 4, 8};
  EXPECT_EQ(outerProduct(lhs, rhs), result);
}

TEST(TestValuesWrapper, OuterProductOfVectorAndCovector) {  // NOLINT
  const ValueMatrix<int, 3, 1> lhs{-1, 3, 4};
  const ValueMatrix<int, 1, 2> rhs{1, 2};
  const ValueMatrix<int, 3, 2> result{-1, -2, 3, 6, 4, 8};
  EXPECT_EQ(outerProduct(lhs, rhs), result);
}

TEST(TestValuesWrapper, OuterProductOfScalarAndMatrix) {  // NOLINT
  const ValueMatrix<int, 1, 1> lhs{2};
  const ValueMatrix<int, 2, 2> rhs{-1, 3, 4, 2};
  const ValueMatrix<int, 2, 2> result{-2, 6, 8, 4};
  EXPECT_EQ(outerProduct(lhs, rhs), result);
}

// The dot product is evaluated, so it does not refer to the temporary transpose of the left-hand side (e.g. with Blitz++).
TEST(TestValuesWrapper, DotProductTypeDerivation) {  // NOLINT
  using A = ValueMatrix<int, 4, 1>;
  static_assert(std::is_same_v<DotProduct<A, A>::type, ValueMatrix<int, 1, 1>>);
}

// The norms keep the value type, also with Blitz++, which widens sums (e.g. of int to long), and std::sqrt, which returns
// double for integers.
TEST(TestValuesWrapper, NormsKeepTheValueType) {  // NOLINT
  const ValueMatrix<int, 2, 1> integers{3, 4};
  static_assert(std::is_same_v<decltype(integers.squaredNorm()), int>);
  static_assert(std::is_same_v<decltype(integers.norm()), int>);
  EXPECT_EQ(integers.squaredNorm(), 25);
  EXPECT_EQ(integers.norm(), 5);
  const ValueMatrix<float, 2, 1> floats{3.0f, 4.0f};
  static_assert(std::is_same_v<decltype(floats.squaredNorm()), float>);
  static_assert(std::is_same_v<decltype(floats.norm()), float>);
  EXPECT_EQ(floats.squaredNorm(), 25.0f);
  EXPECT_EQ(floats.norm(), 5.0f);
}

TEST(TestValuesWrapper, DotProduct) {  // NOLINT
  const ValueMatrix<int, 4, 1> lhs{1, 2, 3, 4};
  const ValueMatrix<int, 4, 1> rhs{0, 3, -1, 2};
  const ValueWrapper<int> result{11};
  EXPECT_EQ(lhs.dotProduct(rhs), result);
}

TEST(TestValuesWrapper, DotProduct1x1Matrices) {  // NOLINT
  const ValueMatrix<int, 1, 1> lhs{1};
  const ValueMatrix<int, 1, 1> rhs{2};
  const ValueWrapper<int> result{2};
  EXPECT_EQ(lhs.dotProduct(rhs), result);
}

TEST(TestValuesWrapper, CrossProduct) {  // NOLINT
  const ValueMatrix<int, 3, 1> lhs{1, 2, 3};
  const ValueMatrix<int, 3, 1> rhs{-1, 3, 4};
  const ValueMatrix<int, 3, 1> result{-1, -7, 5};
  EXPECT_EQ(lhs.crossProduct(rhs), result);
}

TEST(TestValuesWrapper, DivisionWithBuiltinValue) {  // NOLINT
  const ValueMatrix<float, 2, 3> lhs{1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
  const float rhs{3.f};
  const ValueMatrix<float, 2, 3> result{1.f / 3.f, 2.f / 3.f, 1.f, 4.f / 3.f, 5.f / 3.f, 2.f};
  EXPECT_EQ(division(lhs, rhs), result);
  EXPECT_EQ(lhs / rhs, result);
}

TEST(TestValuesWrapper, DivisionWithValue) {  // NOLINT
  const ValueMatrix<float, 2, 3> lhs{1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
  const ValueMatrix<float, 1, 1> rhs{3.f};
  const ValueMatrix<float, 2, 3> result{1.f / 3.f, 2.f / 3.f, 1.f, 4.f / 3.f, 5.f / 3.f, 2.f};
  EXPECT_EQ(division(lhs, rhs), result);
  EXPECT_EQ(lhs / rhs, result);
}

TEST(TestValuesWrapper, DivisionWithValueOnLhsAndRhs) {  // NOLINT
  const ValueMatrix<float, 1, 1> lhs{1.f};
  const ValueMatrix<float, 1, 1> rhs{3.f};
  const ValueMatrix<float, 1, 1> result{1.f / 3.f};
  EXPECT_EQ(division(lhs, rhs), result);
  EXPECT_EQ(lhs / rhs, result);
}

TEST(TestValuesWrapper, Division) {  // NOLINT
  const ValueMatrix<float, 2, 3> lhs{1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
  const ValueMatrix<float, 2, 3> rhs{3.f, 4.f, 1.f, 5.f, 6.f, 2.f};
  const ValueMatrix<float, 2, 3> result{1.f / 3.f, 2.f / 4.f, 3.f, 4.f / 5.f, 5.f / 6.f, 3.f};
  EXPECT_EQ(division(lhs, rhs), result);
  EXPECT_EQ(lhs / rhs, result);
}

// The Blitz++ backend implements neither inverses nor determinants.
#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestValuesWrapper, Inverse) {  // NOLINT
  const ValueMatrix<double, 3, 2> matrix{1.0, 2.0, 4.0, 0.5, 10.0, 0.2};
  const ValueMatrix<double, 3, 2> result{1.0, 0.5, 0.25, 2.0, 0.1, 5.0};
  EXPECT_TRUE(matrix.inverse().isApprox(result));
}

TEST(TestValuesWrapper, TensorInverse) {  // NOLINT
  const ValueMatrix<double, 2, 2> matrix{4.0, 7.0, 2.0, 6.0};
  const ValueMatrix<double, 2, 2> result{0.6, -0.7, -0.2, 0.4};
  EXPECT_TRUE(matrix.tensorInverse().isApprox(result));
}
#endif

TEST(TestValuesWrapper, InitializeMatrix) {  // NOLINT
  const ValueMatrix<int, 3, 2> matrix{1, 2, 3, 4, 5, 6};
  EXPECT_EQ((matrix.get<1, 0>()), 3);
  EXPECT_EQ(matrix.get(1, 0), 3);
}

TEST(TestValuesWrapper, InitializeValue) {  // NOLINT
  const Value<int> value{1};
  EXPECT_EQ(value.get(), 1);
}

TEST(TestValuesWrapper, Evaluate) {  // NOLINT
  const ValueMatrix<int, 2, 3> lhs{1, 2, 3, 4, 5, 6};
  const ValueMatrix<int, 2, 3> rhs{3, 4, 1, 5, 6, 2};
  const ValueMatrix<int, 2, 3> result{4, 6, 4, 9, 11, 8};
  EXPECT_EQ(sum(lhs, rhs).evaluate(), result);
}

// Values have value semantics with every backend: a copy does not share the values of the original, although Blitz++'s copy
// constructor shares them (#60).
TEST(TestValuesWrapper, CopyIsIndependentOfTheOriginal) {  // NOLINT
  ValueMatrix<int, 2, 1> original{1, 2};
  const ValueMatrix<int, 2, 1> copy{original};  // From a non-const object.
  const ValueMatrix<int, 2, 1> copyOfConst{std::as_const(original)};
  ValueMatrix<int, 2, 1> assigned;
  assigned = original;
  const ValueMatrix<int, 2, 1> evaluated = original.evaluate();
  original.set<0, 0>(7);
  const ValueMatrix<int, 2, 1> expected{1, 2};
  EXPECT_EQ(copy, expected);
  EXPECT_EQ(copyOfConst, expected);
  EXPECT_EQ(assigned, expected);
  EXPECT_EQ(evaluated, expected);
  assigned.set<1, 0>(8);
  EXPECT_EQ(original, (ValueMatrix<int, 2, 1>{7, 2}));
}

// Moves hand the values over, and a moved-from object can be assigned to again, so std::swap works, also with Blitz++, whose
// arrays cannot be moved (#60).
TEST(TestValuesWrapper, MoveAndSwap) {  // NOLINT
  ValueMatrix<int, 2, 1> first{1, 2};
  ValueMatrix<int, 2, 1> second{3, 4};
  std::swap(first, second);
  EXPECT_EQ(first, (ValueMatrix<int, 2, 1>{3, 4}));
  EXPECT_EQ(second, (ValueMatrix<int, 2, 1>{1, 2}));
  const ValueMatrix<int, 2, 1> moved{std::move(first)};
  EXPECT_EQ(moved, (ValueMatrix<int, 2, 1>{3, 4}));
  first = second;
  second.set<0, 0>(5);
  EXPECT_EQ(first, (ValueMatrix<int, 2, 1>{1, 2}));
  EXPECT_EQ(moved, (ValueMatrix<int, 2, 1>{3, 4}));
}

// Self-assignment keeps the values, also with Blitz++, whose move empties the moved-from array (#60). The values are
// assigned through an alias, since compilers warn about assigning a variable to itself.
TEST(TestValuesWrapper, SelfAssignment) {  // NOLINT
  ValueMatrix<int, 2, 1> values{1, 2};
  ValueMatrix<int, 2, 1>& alias = values;
  values = alias;
  EXPECT_EQ(values, (ValueMatrix<int, 2, 1>{1, 2}));
  values = std::move(alias);
  EXPECT_EQ(values, (ValueMatrix<int, 2, 1>{1, 2}));
}

// The transpose and a block are copies, not views that share the values of the original, e.g. with Blitz++ (#60).
TEST(TestValuesWrapper, TransposeAndBlockAreIndependentOfTheOriginal) {  // NOLINT
  ValueMatrix<int, 2, 2> original{1, 2, 3, 4};
  ValueMatrix<int, 2, 2> transpose = original.transpose();
  ValueMatrix<int, 2, 1> block = original.getBlock<0, 0, 2, 1>();
  transpose.set<0, 1>(-3);
  block.set<0, 0>(-1);
  EXPECT_EQ(original, (ValueMatrix<int, 2, 2>{1, 2, 3, 4}));
  original.set<1, 0>(5);
  EXPECT_EQ(transpose, (ValueMatrix<int, 2, 2>{1, -3, 2, 4}));
  EXPECT_EQ(block, (ValueMatrix<int, 2, 1>{-1, 3}));
}

// Operations return evaluated values, which do not refer to their operands (#69), while the traits are lazy (see the tests
// Lazy* above).
TEST(TestValuesWrapper, ResultsAreEvaluated) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 2, 3>;
  const MyValueMatrix values{1, 2, 3, 4, 5, 6};
  static_assert(std::is_same_v<decltype(values + values), MyValueMatrix>);
  static_assert(std::is_same_v<decltype(values - values), MyValueMatrix>);
  static_assert(std::is_same_v<decltype(-values), MyValueMatrix>);
  static_assert(std::is_same_v<decltype(values * values), MyValueMatrix>);
  static_assert(std::is_same_v<decltype(values * 2), MyValueMatrix>);
  static_assert(std::is_same_v<decltype(values / values), MyValueMatrix>);
  static_assert(std::is_same_v<decltype(values.getBlock<0, 1, 2, 2>()), ValueMatrix<int, 2, 2>>);
  static_assert(std::is_same_v<decltype(values.innerProduct(values.transpose())), ValueMatrix<int, 2, 2>>);
  // With Eigen 3.4, the evaluated transpose is stored row by row, so its type differs from ValueMatrix<int, 3, 2>.
  using TransposeT = decltype(values.transpose());
  static_assert(std::is_same_v<Evaluate<TransposeT>::type, TransposeT>);
  EXPECT_EQ(values.transpose(), (ValueMatrix<int, 3, 2>{1, 4, 2, 5, 3, 6}));
}

// Operations also work on unevaluated values, i.e. values of the lazy types of the traits (#64). Blitz++ evaluates its
// expressions first where it needs an array, e.g. for the transpose.
TEST(TestValuesWrapper, OperationsOnUnevaluatedValues) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 3, 1>;
  using Implementation = MyValueMatrix::ImplementationT;
  const MyValueMatrix lhs{1, 2, 3};
  const MyValueMatrix rhs{3, 2, 1};
  const Sum<MyValueMatrix, MyValueMatrix>::type unevaluated{
      Sum<AccessImplementation<Implementation>, AccessImplementation<Implementation>>::value(lhs.getImplementation(),
                                                                                             rhs.getImplementation())};
  EXPECT_EQ(unevaluated, (MyValueMatrix{4, 4, 4}));
  Sum<MyValueMatrix, MyValueMatrix>::type copy{unevaluated};
  EXPECT_EQ(copy, (MyValueMatrix{4, 4, 4}));
  const Sum<MyValueMatrix, MyValueMatrix>::type moved{std::move(copy)};
  EXPECT_EQ(moved, (MyValueMatrix{4, 4, 4}));
  EXPECT_EQ(-unevaluated, (MyValueMatrix{-4, -4, -4}));
  EXPECT_EQ(unevaluated.transpose(), (ValueMatrix<int, 1, 3>{4, 4, 4}));
  EXPECT_EQ((unevaluated.getBlock<1, 0, 2, 1>()), (ValueMatrix<int, 2, 1>{4, 4}));
  EXPECT_EQ(unevaluated.dotProduct(lhs), (Value<int>{24}));
  EXPECT_EQ(lhs.transpose().innerProduct(unevaluated), (Value<int>{24}));
  EXPECT_EQ(unevaluated.innerProduct(lhs.transpose()), (ValueMatrix<int, 3, 3>{4, 8, 12, 4, 8, 12, 4, 8, 12}));
  EXPECT_EQ(unevaluated.crossProduct(lhs), (MyValueMatrix{4, -8, 4}));
  EXPECT_EQ(unevaluated.evaluate(), (MyValueMatrix{4, 4, 4}));
}

// Only values of the same size convert implicitly, so that type traits report other conversions as invalid (#62, #70).
// Direct initialization from values of another size fails with a static_assert (compile tests ValuesWrapper.Convert*).
TEST(TestValuesWrapper, ConversionsAreImplicitOnlyIfValid) {  // NOLINT
  using MyValueMatrix = ValueMatrix<int, 3, 1>;
  static_assert(std::is_convertible_v<Sum<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>);
  static_assert(!std::is_convertible_v<ValueMatrix<int, 2, 1>, MyValueMatrix>);
  static_assert(!std::is_convertible_v<ValueMatrix<int, 3, 2>, MyValueMatrix>);
}

namespace {

template <typename ValuesT_>
concept HasGet = requires(const ValuesT_& values) { values.get(); };
template <typename ValuesT_>
concept HasSet = requires(ValuesT_& values) { values.set(1.0); };

}  // namespace

// The members for single values only exist for values of dimension 0: their requires clauses remove them from the others.
TEST(TestValuesWrapper, MembersRequireTheirDimension) {  // NOLINT
  static_assert(HasGet<ValueMatrix<double, 1, 1>>);
  static_assert(!HasGet<ValueMatrix<double, 3, 1>>);
  static_assert(HasSet<ValueMatrix<double, 1, 1>>);
  static_assert(!HasSet<ValueMatrix<double, 3, 1>>);
  static_assert(std::is_convertible_v<ValueMatrix<double, 1, 1>, double>);
  static_assert(!std::is_convertible_v<ValueMatrix<double, 3, 1>, double>);
}

// TODO(remod): Compile-time check number of constructor arguments and test it.
