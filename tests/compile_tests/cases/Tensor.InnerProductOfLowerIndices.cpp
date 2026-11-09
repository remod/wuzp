// wuzp-expect: error
// wuzp-static-assert: An inner product contracts an upper with a lower index, but the last index of the left-hand side and the first index of the right-hand side are both upper or both lower\.

// The inner product contracts the last index of the left-hand side with the first index of the right-hand side. Here, both
// are lower indices, although the tensors have one upper index each.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::innerProduct(TestTensor<Indices<Up, Down>>::zero(), TestTensor<Indices<Down, Up>>::zero());
  return 0;
}
