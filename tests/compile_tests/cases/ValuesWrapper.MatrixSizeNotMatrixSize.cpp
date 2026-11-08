// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The size of a value matrix has rows and columns.

#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/MatrixSize.hpp"

#include "ValueMatrix.hpp"

int main() {
  using Implementation = ValueMatrix<double, 3, 1>::ImplementationT;
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::ValuesWrapper<wuzp::MatrixSize<3>, double, Implementation>);
  return 0;
}
