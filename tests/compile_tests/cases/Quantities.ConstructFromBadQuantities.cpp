// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Unit.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

int main() {
  using MyValueMatrix = ValueMatrix<int, 4, 1>;
  using MyUnitMatrix = TypeMatrix<4, 1, Length, Area, Volume, Mass>;
  using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
  MyQuantityMatrix quantities(QuantityI<Length>(1), QuantityI<Area>(2), QuantityI<Volume>(3), QuantityI<Length>(4));
  return 0;
}
