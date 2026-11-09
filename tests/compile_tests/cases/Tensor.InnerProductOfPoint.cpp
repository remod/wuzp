// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a multiplication\.

// The inner product of a covector and a point depends on the origin, so it is not defined.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result =
      wuzp::innerProduct(TestTensor<Indices<Down>>::zero(), TestTensor<Indices<Up>, wuzp::Absolute>::zero());
  return 0;
}
