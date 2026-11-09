// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a division\.

// The components of a point depend on the origin, so their elementwise quotient is not defined.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result =
      wuzp::cwiseQuotient(TestTensor<Indices<Up>, wuzp::Absolute>::zero(), TestTensor<Indices<Up>>::zero());
  return 0;
}
