// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// The metric is part of the coordinate system: coordinate systems with the same identifier and different metrics do not match.

#include "compile_test_types.hpp"

int main() {
  using EuclideanA = wuzp::CoordinateSystem<SpacetimeAId>;
  [[maybe_unused]] const auto result =
      TestTensor<Indices<Up>, wuzp::Relative, SpacetimeA>::zero() + TestTensor<Indices<Up>, wuzp::Relative, EuclideanA>::zero();
  return 0;
}
