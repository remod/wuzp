#include <gtest/gtest.h>

#include "ctm/eigen.hpp"
#include "ctm/traits.hpp"

using ctm::Sum;

TEST(Eigen_Matrix, IsSame) {  // NOLINT
  using MyValues = Eigen::Matrix<double, 3, 2>;
  static_assert(std::is_same<Sum<MyValues, MyValues>::type, MyValues>::value);
}
