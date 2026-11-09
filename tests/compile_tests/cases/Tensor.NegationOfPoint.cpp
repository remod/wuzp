// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot be negated\.

// The negation of a point is its reflection at the origin, so it depends on the origin.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = -TestTensor<Indices<Up>, wuzp::Absolute>::zero();
  return 0;
}
