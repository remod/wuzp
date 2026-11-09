// wuzp-expect: error
// wuzp-static-assert: Indices do not match\.

// Only tensors with the same indices can be compared. The order of the indices matters: T^i_j and T_j^i are different tensors.

#include "compile_test_types.hpp"

int main() {
  return TestTensor<Indices<Up, Down>>::zero() == TestTensor<Indices<Down, Up>>::zero() ? 0 : 1;
}
