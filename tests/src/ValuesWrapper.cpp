#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"

using namespace wuzp;  // NOLINT

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Value, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Value, Rows, Cols>;
#elif VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_BLITZ
#include "wuzp_blitz/ValueMatrix.hpp"
template <typename Value, int Rows, int Cols>
using ValueMatrix = wuzp_blitz::ValueMatrix<Value, Rows, Cols>;
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

TEST(TestValuesWrapper, LazyProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 2, 2>;
  static_assert(!std::is_same<Product<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValuesWrapper, Initialize) {  // NOLINT
  const ValueMatrix<int, 3, 2> matrix(1, 2, 3, 4, 5, 6);
  EXPECT_EQ((matrix.get<1, 0>()), 3);
}

// TODO(remod): Compile-time check number of constructor arguments and test it.
