// wuzp-expect: error
// wuzp-static-assert: A tensor can only be divided by a scalar, a quantity or a tensor of rank 0\. Use cwiseQuotient for elementwise quotients\.

// Values other than a scalar have no indices, so the elementwise quotient of a tensor by them is not a tensor.

#include "compile_test_types.hpp"

int main() {
  using Vector = wuzp::PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  [[maybe_unused]] const auto result = Vector::zero() / Vector::ValuesT::zero();
  return 0;
}
