// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The units of quantities are a matrix of units, even for a single quantity.

#include "wuzp/core/Quantities.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::Quantities<ValueMatrix<double, 1, 1>, Length>);
  return 0;
}
