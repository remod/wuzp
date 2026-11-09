// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a multiplication\.

// The outer product of a point depends on the origin, so it is not defined, only of vectors.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result =
      wuzp::outerProduct(TestTensor<Indices<Up>, wuzp::Absolute>::zero(), TestTensor<Indices<Down>>::zero());
  return 0;
}
