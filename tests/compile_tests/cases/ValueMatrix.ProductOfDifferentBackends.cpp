// wuzp-expect: error
// wuzp-static-assert: 'Product' trait not specialized for given type\.

// Values of different value matrix backends cannot be multiplied elementwise: the product of a backend with a scalar only
// takes arithmetic values (ArithmeticValue), so it does not take the matrix of the other backend for a scalar.

#include "wuzp_blitz/ValueMatrix.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp_eigen/ValueMatrix.hpp"

int main() {
  const wuzp_eigen::ValueMatrix<double, 3, 1> eigenValues{1.0, 2.0, 3.0};
  const wuzp_blitz::ValueMatrix<double, 3, 1> blitzValues{1.0, 2.0, 3.0};
  [[maybe_unused]] const auto product = eigenValues * blitzValues;
  return 0;
}
