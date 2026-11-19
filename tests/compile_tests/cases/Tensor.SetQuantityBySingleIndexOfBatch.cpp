// wuzp-expect: error
// wuzp-static-assert: A batch of vectors, one per column, only supports transformations, sums, differences, comparisons, and products and quotients with scalars\.

// The single index of a batch of vectors would not select a component of a vector, so batches reject it, also batches of
// dimension 2, e.g. a point cloud, for which the requires clause keeps the member, unlike for a matrix, so that they fail with
// this message. setQuantity<RowIdV_, ColIdV_>() sets the component RowIdV_ of the member ColIdV_.

#include "compile_test_types.hpp"

int main() {
  auto batch = TestBatch<3>::zero();
  batch.setQuantity<1>(QuantityD<Length>{1.0});
  return 0;
}
