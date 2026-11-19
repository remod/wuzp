// wuzp-expect: error
// wuzp-static-assert: Element row id is out of range\.

// The row and the column of set<RowIdV_, ColIdV_>() are checked at compile time, like those of get<RowIdV_, ColIdV_>().

#include "ValueMatrix.hpp"

int main() {
  auto column = ValueMatrix<double, 2, 1>::zero();
  column.set<2, 0>(1.0);
  return 0;
}
