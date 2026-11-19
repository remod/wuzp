// wuzp-expect: error
// wuzp-static-assert: A batch of vectors, one per column, only supports transformations, sums, differences, comparisons, and products and quotients with scalars\.

// The single index of a batch of vectors would not select a component of a vector, so batches reject it, also with an index
// known at run time, and also batches of dimension 2, e.g. a point cloud, for which the requires clause keeps the member, unlike
// for a matrix, so that they fail with this message. getQuantity(rowId, colId) selects the component rowId of the member colId.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto quantity = TestBatch<3>::zero().getQuantity(1);
  return 0;
}
