// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The values of quantities are a value matrix of a value matrix backend, not e.g. a plain number.

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::Quantities<double, wuzp::TypeMatrix<1, 1, Length>>);
  return 0;
}
