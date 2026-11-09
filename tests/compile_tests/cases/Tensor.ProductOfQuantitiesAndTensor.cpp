// wuzp-expect: error
// wuzp-static-assert: A tensor can only be multiplied by a scalar, a quantity or a tensor of rank 0\. Use innerProduct, outerProduct or cwiseProduct for other products\.

// Quantities other than a single quantity have no indices, so their elementwise product with a tensor is not a tensor.

#include "compile_test_types.hpp"

int main() {
  using Vector = wuzp::PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  [[maybe_unused]] const auto result = Vector::QuantitiesT::zero() * Vector::zero();
  return 0;
}
