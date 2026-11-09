// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a division\.

// Dividing a point is not defined.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>, wuzp::Absolute>::zero() / TestTensor<Indices<>>::zero();
  return 0;
}
