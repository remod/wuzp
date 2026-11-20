// wuzp-expect: error
// wuzp-static-assert: dotProduct, norm, squaredNorm, crossProduct and the transpose of a vector or a covector use the Euclidean metric, which this coordinate system does not have\. Pair a covector and a vector with innerProduct, which needs no metric, or contract with the metric explicitly, e\.g\. with getInvariant\(\) in a Minkowski coordinate system\.

// The skew-symmetric matrix is the matrix of the cross product, which uses the Levi-Civita symbol of a Euclidean coordinate
// system (#77).

#include "compile_test_types.hpp"

int main() {
  const wuzp::SpacetimeVector3<ValueMatrix, double, SpacetimeA> vector{1.0, 2.0, 3.0};
  [[maybe_unused]] const auto skew = wuzp::skewSymmetricMatrix(vector);
  return 0;
}
