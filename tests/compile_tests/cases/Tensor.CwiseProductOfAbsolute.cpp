// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a multiplication\.

// The components of a point depend on the origin, so their elementwise product is not defined.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::cwiseProduct(TestTensor<Indices<Up>, wuzp::Absolute>::zero(), TestTensor<Indices<Up>>::zero());
  return 0;
}
