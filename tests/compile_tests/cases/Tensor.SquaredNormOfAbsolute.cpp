// wuzp-expect: error
// wuzp-static-assert: Tensor is not relative\.

// The squared norm of a point is not defined, only the squared norm of a vector.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto norm = TestTensor<Indices<Up>, wuzp::Absolute>::zero().squaredNorm();
  return 0;
}
