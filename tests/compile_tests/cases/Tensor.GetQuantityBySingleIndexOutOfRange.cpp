// wuzp-expect: error
// wuzp-static-assert: Element row id is out of range\.

// The single index of a vector is its row, which is checked at compile time, like the row and the column of
// getQuantity<RowIdV_, ColIdV_>().

#include "compile_test_types.hpp"

int main() {
  const auto vector = wuzp::PositionVector3<ValueMatrix, double, CoordinateSystemA>::zero();
  [[maybe_unused]] const auto quantity = vector.getQuantity<3>();
  return 0;
}
