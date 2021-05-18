#include <gtest/gtest.h>

#include "ctm/core/traits.hpp"
#include "ctm/eigen/eigen.hpp"

using ctm::Block;
using ctm::Transpose;
using ctm::ValuesDifference;
using ctm::ValuesProduct;
using ctm::ValuesSum;

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
  static_assert(!std::is_same<ValuesSum<MyValues, MyValues>::type, MyValues>::value);
}

TEST(EigenMatrix, LazyDifference) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<ValuesDifference<MyValues, MyValues>::type, MyValues>::value);
}

TEST(EigenMatrix, LazyProduct) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<ValuesProduct<MyValues, MyValues>::type, MyValues>::value);
}
