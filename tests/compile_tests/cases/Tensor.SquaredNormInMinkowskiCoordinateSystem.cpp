// wuzp-expect: error
// wuzp-static-assert: dotProduct, norm, squaredNorm, crossProduct and the transpose of a vector or a covector use the Euclidean metric, which this coordinate system does not have\. Pair a covector and a vector with innerProduct, which needs no metric, or contract with the metric explicitly, e\.g\. with getInvariant\(\) in a Minkowski coordinate system\.

// The squared norm of a covector uses the Euclidean metric, which a Minkowski coordinate system does not have.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Down>, wuzp::Relative, SpacetimeA>::zero().squaredNorm();
  return 0;
}
