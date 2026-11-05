// wuzp-expect: error
// wuzp-static-assert: Absolute/relative mismatch\.

// A point cannot be compared with a vector.

#include "compile_test_types.hpp"

int main() {
  return TestTensor<1, 0, wuzp::Absolute>::zero() == TestTensor<1, 0>::zero() ? 0 : 1;
}
