// wuzp-expect: error
// wuzp-static-assert: Element row id is out of range\.

// A one-dimensional vector is stored as a 1x1 matrix: its only component has the single index 0, so the index 1 is out of range
// and rejected at compile time, like the row and the column of getQuantity<RowIdV_, ColIdV_>().

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto quantity = TestTensor<Indices<Up>>::zero().getQuantity<1>();
  return 0;
}
