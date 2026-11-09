// wuzp-expect: error
// wuzp-static-assert: 'Sum' trait not specialized for given type\.

// Several value matrix backends can be used in one program, but values of different backends cannot be combined: each backend
// only specializes the traits for its own matrices.

#include "wuzp_blitz/ValueMatrix.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp_eigen/ValueMatrix.hpp"

int main() {
  const wuzp_eigen::ValueMatrix<double, 3, 1> eigenValues{1.0, 2.0, 3.0};
  const wuzp_blitz::ValueMatrix<double, 3, 1> blitzValues{1.0, 2.0, 3.0};
  [[maybe_unused]] const auto sum = eigenValues + blitzValues;
  return 0;
}
