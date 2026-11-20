// wuzp-expect: error
// wuzp-static-assert: dotProduct, norm, squaredNorm, crossProduct and the transpose of a vector or a covector use the Euclidean metric, which this coordinate system does not have\. Pair a covector and a vector with innerProduct, which needs no metric, or contract with the metric explicitly, e\.g\. with getInvariant\(\) in a Minkowski coordinate system\.

// The vector of a skew-symmetric matrix inverts the matrix of the cross product, which uses the Levi-Civita symbol of a
// Euclidean coordinate system (#77).

#include "compile_test_types.hpp"

int main() {
  using LinearMap = wuzp::Tensor<Indices<Up, Down>, wuzp::Quantities<ValueMatrix<double, 3, 3>, wuzp::TypeMatrix<3, 3, Length>>, SpacetimeA,
                                 wuzp::Relative>;
  const LinearMap map{0.0, -3.0, 2.0, 3.0, 0.0, -1.0, -2.0, 1.0, 0.0};
  [[maybe_unused]] const auto vector = wuzp::vectorOfSkewSymmetricMatrix(map);
  return 0;
}
