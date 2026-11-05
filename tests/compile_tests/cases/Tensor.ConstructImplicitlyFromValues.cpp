// wuzp-expect: error
// wuzp-error: (no viable )?conversion from

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

struct CoordinateSystemAName {};
using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
using MyValueMatrix = ValueMatrix<int, 2, 3>;
using MyUnitMatrix = TypeMatrix<2, 3, Length>;
using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
using MyTensor = Tensor<1, 0, MyQuantityMatrix, CoordinateSystemA, Relative>;

int main() {
  const MyValueMatrix values{1, 2, 3, 4, 5, 6};
  // Use = instead of {} to trigger implicit conversion.
  [[maybe_unused]] const MyTensor tensor = values;
  return 0;
}
