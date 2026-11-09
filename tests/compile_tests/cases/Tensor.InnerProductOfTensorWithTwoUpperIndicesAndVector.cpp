// wuzp-expect: error
// wuzp-static-assert: An inner product contracts an upper with a lower index, but the last index of the left-hand side and the first index of the right-hand side are both upper or both lower\.

// A tensor with two upper indices, e.g. a covariance matrix T^ij, contracts its second index with a covector, not with a vector.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::innerProduct(TestTensor<Indices<Up, Up>>::zero(), TestTensor<Indices<Up>>::zero());
  return 0;
}
