// wuzp-expect: error
// wuzp-static-assert: Absolute/relative mismatch\.

// A vector cannot be converted into a point, not even explicitly; makeAbsolute() does it.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const TestTensor<Indices<Up>, wuzp::Absolute> tensor{TestTensor<Indices<Up>>::zero()};
  return 0;
}
