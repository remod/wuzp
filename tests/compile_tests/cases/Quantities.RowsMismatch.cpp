// wuzp-expect: error
// wuzp-static-assert: Number of rows do not match\.

// The value matrix and the unit matrix must have the same number of rows.

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::Quantities<ValueMatrix<double, 2, 1>, wuzp::TypeMatrix<3, 1, Length>>);
  return 0;
}
