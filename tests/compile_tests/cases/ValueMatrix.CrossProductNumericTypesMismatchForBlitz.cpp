// wuzp-expect: error
// wuzp-requires: values=blitz
// wuzp-static-assert: Numeric types do not match\.

// With Blitz++, the cross product requires both vectors to have the same numeric type. Through the public API, the generic
// check of the value types reports the problem first, so the backend trait is used directly.

#include "ValueMatrix.hpp"

int main() {
  using Lhs = ValueMatrix<double, 3, 1>::ImplementationT;
  using Rhs = ValueMatrix<float, 3, 1>::ImplementationT;
  const Lhs lhs;
  const Rhs rhs;
  [[maybe_unused]] const auto result =
      wuzp::CrossProduct<wuzp::AccessImplementation<Lhs>, wuzp::AccessImplementation<Rhs>>::value(lhs, rhs);
  return 0;
}
