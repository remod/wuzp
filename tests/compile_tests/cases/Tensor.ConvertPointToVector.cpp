// wuzp-expect: error
// wuzp-static-assert: Absolute/relative mismatch\.

// A point cannot be converted into a vector, not even explicitly; makeRelative() does it. Direct initialization explains why
// the conversion fails, while copy initialization does not consider it (Tensor.ConvertPointToVectorImplicitly).

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const TestTensor<Indices<Up>> tensor{TestTensor<Indices<Up>, wuzp::Absolute>::zero()};
  return 0;
}
