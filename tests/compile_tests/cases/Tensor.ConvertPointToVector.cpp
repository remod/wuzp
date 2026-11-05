// wuzp-expect: error
// wuzp-static-assert: Absolute/relative mismatch\.

// A point cannot be converted into a vector implicitly; makeRelative() does it explicitly.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const TestTensor<1, 0> tensor = TestTensor<1, 0, wuzp::Absolute>::zero();
  return 0;
}
