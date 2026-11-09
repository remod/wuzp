// wuzp-expect: error
// wuzp-static-assert: Indices do not match\.

// A tensor cannot be converted into a tensor with other indices. The order of the indices matters: T^i_j and T_j^i are different tensors.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const TestTensor<Indices<Up, Down>> tensor{TestTensor<Indices<Down, Up>>::zero()};
  return 0;
}
