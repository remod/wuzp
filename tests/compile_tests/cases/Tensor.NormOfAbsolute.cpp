// wuzp-expect: error
// wuzp-static-assert: The norm of a point depends on the origin\. Use the norm of a difference of points, e\.g\. \(p - q\)\.norm\(\)\.

// The norm of a point is its distance from the origin, so it depends on the origin. The distance of two points is the norm of
// their difference, a vector.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto norm = TestTensor<Indices<Up>, wuzp::Absolute>::zero().norm();
  return 0;
}
