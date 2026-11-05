// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a multiplication\.

// Multiplying a point is not defined.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<1, 0, wuzp::Absolute>::zero() * TestTensor<1, 0>::zero();
  return 0;
}
