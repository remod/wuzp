// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

struct CoordinateSystemAName {};
struct CoordinateSystemBName {};
using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
using CoordinateSystemB = CoordinateSystem<CoordinateSystemBName>;
using MyValueMatrix = ValueMatrix<int, 4, 1>;
using MyUnitMatrix = TypeMatrix<4, 1, Length>;
using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
using MyTensorLhs = Vector<MyQuantityMatrix, CoordinateSystemA>;
using MyTensorRhs = Vector<MyQuantityMatrix, CoordinateSystemB>;

int main() {
  MyTensorLhs{1, 2, 3, 4}.dotProduct(MyTensorRhs{0, 3, -1, 2});
  return 0;
}
