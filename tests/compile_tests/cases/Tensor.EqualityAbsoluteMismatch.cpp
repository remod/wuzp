// wuzp-expect: error
// wuzp-static-assert: Absolute/relative mismatch\.

// A point cannot be compared with a vector.

#include "compile_test_types.hpp"

int main() {
  return TestTensor<Indices<Up>, wuzp::Absolute>::zero() == TestTensor<Indices<Up>>::zero() ? 0 : 1;
}
