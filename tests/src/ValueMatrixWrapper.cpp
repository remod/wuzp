#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"

using namespace wuzp;  // NOLINT

#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;

TEST(TestValueMatrix, Dimension) {  // NOLINT
  static_assert(Dimension<ValueMatrix<double, 1, 1>>::value == 0);
  static_assert(Dimension<ValueMatrix<double, 3, 1>>::value == 1);
  static_assert(Dimension<ValueMatrix<double, 1, 2>>::value == 1);
  static_assert(Dimension<ValueMatrix<double, 3, 2>>::value == 2);
}

TEST(TestValueMatrix, Rows) {  // NOLINT
  static_assert(Rows<ValueMatrix<double, 3, 2>>::value == 3);
}

TEST(TestValueMatrix, Cols) {  // NOLINT
  static_assert(Cols<ValueMatrix<double, 3, 2>>::value == 2);
}

TEST(TestValueMatrix, LazyBlock) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<Block<MyValueMatrix, 0, 0, 1, 1>::type, MyValueMatrix>::value);
}

TEST(TestValueMatrix, LazyTranspose) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<Transpose<MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValueMatrix, LazySum) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<Sum<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValueMatrix, LazyDifference) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<Difference<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValueMatrix, LazyProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<Product<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}

// TODO(remod): Compile-time check number of constructor arguments and test it.

#endif
