// wuzp-expect: error
// wuzp-static-assert: An inner product contracts an upper with a lower index, but the last index of the left-hand side and the first index of the right-hand side are both upper or both lower\.

// A covector and a bilinear form, e.g. a mass matrix M_ij, both have lower indices, so they cannot be contracted.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::innerProduct(TestTensor<Indices<Down>>::zero(), TestTensor<Indices<Down, Down>>::zero());
  return 0;
}
