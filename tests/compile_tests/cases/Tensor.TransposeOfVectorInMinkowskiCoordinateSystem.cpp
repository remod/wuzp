// wuzp-expect: error
// wuzp-static-assert: dotProduct, norm, squaredNorm, crossProduct and the transpose of a vector or a covector use the Euclidean metric, which this coordinate system does not have\. Pair a covector and a vector with innerProduct, which needs no metric, or contract with the metric explicitly, e\.g\. with getInvariant\(\) in a Minkowski coordinate system\.

// The transpose of a vector lowers its index with the Euclidean metric, which keeps the components. In a Minkowski coordinate
// system, the metric eta lowers it explicitly: innerProduct(eta, v).

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>, wuzp::Relative, SpacetimeA>::zero().transpose();
  return 0;
}
