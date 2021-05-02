#include <gtest/gtest.h>

#include "ctm/eigen.hpp"
#include "ctm/traits.hpp"

using ctm::Sum;
using ctm::Transpose;

TEST(EigenMatrix, LazyTranspose) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<Transpose<MyValues>::type, MyValues>::value);
}

TEST(EigenMatrix, LazySum) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(!std::is_same<Sum<MyValues, MyValues>::type, MyValues>::value);
}
