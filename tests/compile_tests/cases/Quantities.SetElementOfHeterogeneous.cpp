// wuzp-expect: error
// wuzp-static-assert: Quantities are not homogeneous\.

// Setting an element by runtime indices requires all elements to have the same unit, since the unit cannot be checked at runtime.

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  using Heterogeneous = wuzp::Quantities<ValueMatrix<double, 1, 2>, wuzp::TypeMatrix<1, 2, Length, Time>>;
  Heterogeneous quantities;
  quantities.set(0, 0, QuantityD<Length>{1.0});
  return 0;
}
