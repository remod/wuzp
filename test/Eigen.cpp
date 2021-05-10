#include <gtest/gtest.h>

#include "ctm/eigen.hpp"
#include "ctm/traits.hpp"

using ctm::Block;
using ctm::Difference;
using ctm::Product;
using ctm::Sum;
using ctm::Transpose;

TEST(EigenMatrix, LazyBlock) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<Block<MyValues, 0, 0, 1, 1>::type, MyValues>::value);
}

TEST(EigenMatrix, LazyTranspose) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<Transpose<MyValues>::type, MyValues>::value);
}

TEST(EigenMatrix, LazySum) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<Sum<MyValues, MyValues>::type, MyValues>::value);
}

TEST(EigenMatrix, LazyDifference) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<Difference<MyValues, MyValues>::type, MyValues>::value);
}

TEST(EigenMatrix, LazyProduct) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<Product<MyValues, MyValues>::type, MyValues>::value);
}
