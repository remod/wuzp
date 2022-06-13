#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"

using namespace wuzp;  // NOLINT

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/Values.hpp"
template <typename Scalar, int Rows, int Cols>
using Values = wuzp_eigen::Values<Scalar, Rows, Cols>;

TEST(TestValuesWrapper, Dimension) {  // NOLINT
  static_assert(Dimension<Values<double, 1, 1>>::value == 0);
  static_assert(Dimension<Values<double, 3, 1>>::value == 1);
  static_assert(Dimension<Values<double, 1, 2>>::value == 1);
  static_assert(Dimension<Values<double, 3, 2>>::value == 2);
}

TEST(TestValuesWrapper, Rows) {  // NOLINT
  static_assert(Rows<Values<double, 3, 2>>::value == 3);
}

TEST(TestValuesWrapper, Cols) {  // NOLINT
  static_assert(Cols<Values<double, 3, 2>>::value == 2);
}

TEST(TestValuesWrapper, LazyBlock) {  // NOLINT
  using MyValues = Values<double, 3, 2>;
  static_assert(!std::is_same<Block<MyValues, 0, 0, 1, 1>::type, MyValues>::value);
}

TEST(TestValuesWrapper, LazyTranspose) {  // NOLINT
  using MyValues = Values<double, 3, 2>;
  static_assert(!std::is_same<Transpose<MyValues>::type, MyValues>::value);
}

TEST(TestValuesWrapper, LazySum) {  // NOLINT
  using MyValues = Values<double, 3, 2>;
  static_assert(!std::is_same<Sum<MyValues, MyValues>::type, MyValues>::value);
}

TEST(TestValuesWrapper, LazyDifference) {  // NOLINT
  using MyValues = Values<double, 3, 2>;
  static_assert(!std::is_same<Difference<MyValues, MyValues>::type, MyValues>::value);
}

TEST(TestValuesWrapper, LazyProduct) {  // NOLINT
  using MyValues = Values<double, 3, 2>;
  static_assert(!std::is_same<Product<MyValues, MyValues>::type, MyValues>::value);
}

// TODO(remod): Compile-time check number of constructor arguments and test it.

#endif
