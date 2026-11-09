// wuzp-expect: error
// wuzp-error(compiler=gnu|clang|appleclang): (no viable )?conversion from
// wuzp-error(compiler=msvc): cannot convert from

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

struct CoordinateSystemAName {};
using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
using MyValueMatrix = ValueMatrix<int, 3, 1>;
using MyUnitMatrix = TypeMatrix<3, 1, Length>;
using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;
using MyTensor = Tensor<Indices<Up>, MyQuantityMatrix, CoordinateSystemA, Relative>;

int main() {
  const MyValueMatrix values{1, 2, 3};
  // Use = instead of {} to trigger implicit conversion.
  [[maybe_unused]] const MyTensor tensor = values;
  return 0;
}
