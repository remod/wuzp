#include <gtest/gtest.h>

#include "ctm/core/traits.hpp"
#include "ctm/eigen/eigen.hpp"

using Traits = ctm::ValuesTraits;

TEST(EigenMatrix, LazyBlock) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<Traits::Block<MyValues, 0, 0, 1, 1>::type, MyValues>::value);
}

TEST(EigenMatrix, LazyTranspose) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<Traits::Transpose<MyValues>::type, MyValues>::value);
}

TEST(EigenMatrix, LazySum) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<Traits::Sum<MyValues, MyValues>::type, MyValues>::value);
}

TEST(EigenMatrix, LazyDifference) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<Traits::Difference<MyValues, MyValues>::type, MyValues>::value);
}

TEST(EigenMatrix, LazyProduct) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<Traits::Product<MyValues, MyValues>::type, MyValues>::value);
}
