// wuzp-expect: error
// wuzp-static-assert: Absolute/relative mismatch\.

// The dot product of a point and a vector is not defined.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<1, 0, wuzp::Absolute>::zero().dotProduct(TestTensor<1, 0>::zero());
  return 0;
}
