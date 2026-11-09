// wuzp-expect: error
// wuzp-static-assert: Absolute/relative mismatch\.

// An absolute tensor (a point) cannot be compared with a relative one (a vector).

#include "compile_test_types.hpp"

int main() {
  return TestTensor<Indices<Up>, wuzp::Absolute>::zero().isApprox(TestTensor<Indices<Up>>::zero()) ? 0 : 1;
}
