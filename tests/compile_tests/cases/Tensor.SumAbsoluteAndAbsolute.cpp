// wuzp-expect: error
// wuzp-static-assert: Two absolute objects cannot be summed\.

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Unit.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;

struct CoordinateSystemAName {};
using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
using MyValueMatrix = ValueMatrix<int, 2, 3>;
using MyUnitMatrix = TypeMatrix<2, 3, Length>;
using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
using MyAbsoluteTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Absolute>;

int main() {
  const MyAbsoluteTensor tensor{1, 4, 2, 5, 3, 6};
  tensor + tensor;
  return 0;
}
