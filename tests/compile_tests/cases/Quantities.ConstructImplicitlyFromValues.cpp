// wuzp-expect: error
// wuzp-error(compiler=gnu|clang|appleclang): (no viable )?conversion from
// wuzp-error(compiler=msvc): cannot convert from

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

using namespace wuzp;  // NOLINT

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using MyValueMatrix = ValueMatrix<int, 2, 3>;
using MyUnitMatrix = TypeMatrix<2, 3, Length>;
using MyQuantityMatrix = Quantities<MyValueMatrix, MyUnitMatrix>;

int main() {
  const MyValueMatrix values{1, 2, 3, 4, 5, 6};
  // Use = instead of {} to trigger implicit conversion.
  [[maybe_unused]] const MyQuantityMatrix quantities = values;
  return 0;
}
