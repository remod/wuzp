#include <gtest/gtest.h>

#include "ctm/core/traits.hpp"
#include "ctm/value_matrix/ValueMatrix.hpp"
#include "ctm/value_matrix/ValueMatrixTraits.hpp"

using namespace ctm::value_matrix;  // NOLINT

TEST(TestValueMatrix, LazyBlock) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<ValueMatrixTraits::Block<MyValueMatrix, 0, 0, 1, 1>::type, MyValueMatrix>::value);
}

TEST(TestValueMatrix, LazyTranspose) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<ValueMatrixTraits::Transpose<MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValueMatrix, LazySum) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<ValueMatrixTraits::Sum<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValueMatrix, LazyDifference) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<ValueMatrixTraits::Difference<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}

TEST(TestValueMatrix, LazyProduct) {  // NOLINT
  using MyValueMatrix = ValueMatrix<double, 3, 2>;
  static_assert(!std::is_same<ValueMatrixTraits::Product<MyValueMatrix, MyValueMatrix>::type, MyValueMatrix>::value);
}
