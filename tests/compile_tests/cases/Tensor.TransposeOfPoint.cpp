// wuzp-expect: error
// wuzp-static-assert: Only a tensor with one upper index, a vector or a batch of vectors, can be absolute \(a point\)\.

// The transpose of a vector is a covector, but there are no absolute covectors: the components of a point depend on the origin.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>, wuzp::Absolute>::zero().transpose();
  return 0;
}
