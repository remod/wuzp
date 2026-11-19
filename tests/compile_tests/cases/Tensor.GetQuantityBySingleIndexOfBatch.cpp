// wuzp-expect: error
// wuzp-static-assert: A batch of vectors, one per column, only supports transformations, sums, differences, comparisons, and products and quotients with scalars\.

// A batch of one-dimensional points, e.g. of time points, is stored as a row, so it has dimension 1 like a covector: its single
// index would select a member of the batch rather than a component, so batches reject it. getQuantity<RowIdV_, ColIdV_>()
// selects a component of a member.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto quantity = TestBatch<1, wuzp::Absolute>::zero().getQuantity<1>();
  return 0;
}
