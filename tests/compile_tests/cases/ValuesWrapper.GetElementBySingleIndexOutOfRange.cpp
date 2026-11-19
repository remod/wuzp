// wuzp-expect: error
// wuzp-static-assert: Element columns id is out of range\.

// The single index of a row is its column, which is checked at compile time, like the row and the column of
// get<RowIdV_, ColIdV_>(): values check them themselves, since they have no units whose element would.

#include "ValueMatrix.hpp"

int main() {
  const ValueMatrix<double, 1, 3> row{1.0, 2.0, 3.0};
  [[maybe_unused]] const auto element = row.get<3>();
  return 0;
}
