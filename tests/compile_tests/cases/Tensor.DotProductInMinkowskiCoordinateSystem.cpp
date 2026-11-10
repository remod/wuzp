// wuzp-expect: error
// wuzp-static-assert: dotProduct, norm, squaredNorm, crossProduct and the transpose of a vector or a covector use the Euclidean metric, which this coordinate system does not have\. Pair a covector and a vector with innerProduct, which needs no metric, or contract with the metric explicitly, e\.g\. with getInvariant\(\) in a Minkowski coordinate system\.

// The dot product of two vectors uses the Euclidean metric, which a Minkowski coordinate system does not have: the Euclidean
// dot product of two spacetime vectors changes under a Lorentz transformation.

#include "compile_test_types.hpp"

int main() {
  using Vector = TestTensor<Indices<Up>, wuzp::Relative, SpacetimeA>;
  [[maybe_unused]] const auto result = Vector::zero().dotProduct(Vector::zero());
  return 0;
}
