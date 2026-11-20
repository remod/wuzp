// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// Both elements of each pair of a skew-symmetric matrix, e.g. M_21 and M_12, have the unit of a component of its vector: here
// the rows of the linear map have different units (#77).

#include "compile_test_types.hpp"

int main() {
  using LinearMap = wuzp::Tensor<
      Indices<Up, Down>,
      wuzp::Quantities<ValueMatrix<double, 3, 3>, wuzp::TypeMatrix<3, 3, Length, Length, Length, Length, Length, Length, Time, Time, Time>>,
      CoordinateSystemA, wuzp::Relative>;
  const LinearMap map{0.0, -3.0, 2.0, 3.0, 0.0, -1.0, -2.0, 1.0, 0.0};
  [[maybe_unused]] const auto vector = wuzp::vectorOfSkewSymmetricMatrix(map);
  return 0;
}
