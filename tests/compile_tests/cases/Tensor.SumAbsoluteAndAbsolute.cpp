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
using MyValueMatrix = ValueMatrix<int, 3, 1>;
using MyUnitMatrix = TypeMatrix<3, 1, Length>;
using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
using MyAbsoluteTensor = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Absolute>;

int main() {
  const MyAbsoluteTensor tensor{1, 4, 2};
  tensor + tensor;
  return 0;
}
