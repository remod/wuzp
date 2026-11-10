// wuzp-expect: error
// wuzp-static-assert: A batch of vectors, one per column, only supports transformations, sums, differences, comparisons, and products and quotients with scalars\.

// The Minkowski invariant is defined for a single vector, not for a batch of vectors.

#include "wuzp/trafo/LorentzTransformation.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::getInvariant(TestBatch<4, wuzp::Relative, SpacetimeA>::zero());
  return 0;
}
