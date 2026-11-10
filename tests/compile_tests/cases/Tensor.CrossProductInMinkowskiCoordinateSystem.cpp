// wuzp-expect: error
// wuzp-static-assert: dotProduct, norm, squaredNorm, crossProduct and the transpose of a vector or a covector use the Euclidean metric, which this coordinate system does not have\. Pair a covector and a vector with innerProduct, which needs no metric, or contract with the metric explicitly, e\.g\. with getInvariant\(\) in a Minkowski coordinate system\.

// The cross product uses the Euclidean metric of an orthonormal basis, which a Minkowski coordinate system does not have.

#include "compile_test_types.hpp"

int main() {
  using Vector = wuzp::HomogeneousVector<3, ValueMatrix, double, Length, SpacetimeA>;
  [[maybe_unused]] const auto result = Vector::zero().crossProduct(Vector::zero());
  return 0;
}
