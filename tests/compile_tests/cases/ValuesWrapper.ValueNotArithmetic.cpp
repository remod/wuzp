// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The elements of a value matrix are numbers, not e.g. quantities, which already have a value matrix.

#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/Quantity.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  using Implementation = ValueMatrix<double, 3, 1>::ImplementationT;
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::ValuesWrapper<wuzp::MatrixSize<3, 1>, QuantityD<Length>, Implementation>);
  return 0;
}
