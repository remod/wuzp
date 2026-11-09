// wuzp-expect: error
// wuzp-static-assert: Indices do not match\.

// Only tensors of the same type can be compared approximately. The order of the indices matters: T^i_j and T_j^i are different tensors.

#include "compile_test_types.hpp"

int main() {
  return TestTensor<Indices<Up, Down>>::zero().isApprox(TestTensor<Indices<Down, Up>>::zero()) ? 0 : 1;
}
