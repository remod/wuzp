// wuzp-expect: error
// wuzp-static-assert: The norm of a point depends on the origin\. Use the norm of a difference of points, e\.g\. \(p - q\)\.norm\(\)\.

// The squared norm of a point depends on the origin, like its norm, so it is rejected with the same message.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto norm = TestTensor<Indices<Up>, wuzp::Absolute>::zero().squaredNorm();
  return 0;
}
