// wuzp-expect: error
// wuzp-static-assert: An inner product contracts an upper with a lower index, but the last index of the left-hand side and the first index of the right-hand side are both upper or both lower\.

// A linear map T^i_j contracts its lower index with a vector, not with a covector, which has a lower index too.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::innerProduct(TestTensor<Indices<Up, Down>>::zero(), TestTensor<Indices<Down>>::zero());
  return 0;
}
