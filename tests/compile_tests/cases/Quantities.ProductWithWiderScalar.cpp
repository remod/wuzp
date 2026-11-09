// wuzp-expect: error
// wuzp-static-assert: The common type of the scalar and the value type is not the value type, e\.g\. a double scalar with int values\.

// A scalar is converted into the value type of the quantities, e.g. 2 * q for quantities of doubles, but not if this narrows it.

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  using Lengths = wuzp::Quantities<ValueMatrix<int, 3, 1>, wuzp::TypeMatrix<3, 1, Length>>;
  [[maybe_unused]] const auto result = 2.5 * Lengths::zero();
  return 0;
}
