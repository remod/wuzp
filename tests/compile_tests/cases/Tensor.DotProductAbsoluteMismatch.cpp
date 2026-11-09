// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a multiplication\.

// The dot product of a point and a vector depends on the origin, so it is not defined.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>, wuzp::Absolute>::zero().dotProduct(TestTensor<Indices<Up>>::zero());
  return 0;
}
