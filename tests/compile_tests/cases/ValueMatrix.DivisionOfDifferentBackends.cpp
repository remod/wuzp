// wuzp-expect: error
// wuzp-static-assert: 'Division' trait not specialized for given type\.

// Values of different value matrix backends cannot be divided elementwise: the division of a backend by a scalar only takes
// arithmetic values (ArithmeticValue), so it does not take the matrix of the other backend for a scalar.

#include "wuzp_blitz/ValueMatrix.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp_eigen/ValueMatrix.hpp"

int main() {
  const wuzp_blitz::ValueMatrix<double, 3, 1> blitzValues{1.0, 2.0, 3.0};
  const wuzp_eigen::ValueMatrix<double, 3, 1> eigenValues{1.0, 2.0, 3.0};
  [[maybe_unused]] const auto quotient = blitzValues / eigenValues;
  return 0;
}
