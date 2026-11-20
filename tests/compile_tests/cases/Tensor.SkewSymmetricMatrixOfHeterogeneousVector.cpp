// wuzp-expect: error
// wuzp-static-assert: Quantities are not homogeneous\.

// The elements of the skew-symmetric matrix of a vector have the unit of its components, which must be the same (#77).

#include "compile_test_types.hpp"

int main() {
  using Vector = wuzp::Vector<wuzp::Quantities<ValueMatrix<double, 3, 1>, wuzp::TypeMatrix<3, 1, Length, Length, Time>>, CoordinateSystemA>;
  const Vector vector{1.0, 2.0, 3.0};
  [[maybe_unused]] const auto skew = wuzp::skewSymmetricMatrix(vector);
  return 0;
}
