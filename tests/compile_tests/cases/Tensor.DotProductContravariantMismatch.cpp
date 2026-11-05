// wuzp-expect: error
// wuzp-static-assert: Contravariant indices do not match\.

// The dot product requires tensors with the same indices.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<1, 0>::zero().dotProduct(TestTensor<0, 0>::zero());
  return 0;
}
