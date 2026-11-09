// wuzp-expect: error
// wuzp-static-assert: An inner product contracts an upper with a lower index, but the last index of the left-hand side and the first index of the right-hand side are both upper or both lower\.

// The inner product of a vector and a linear map T^i_j would contract two upper indices: that of the vector and the first index of
// the linear map. A covector w_i contracts with it, w_i T^i_j.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::innerProduct(TestTensor<Indices<Up>>::zero(), TestTensor<Indices<Up, Down>>::zero());
  return 0;
}
