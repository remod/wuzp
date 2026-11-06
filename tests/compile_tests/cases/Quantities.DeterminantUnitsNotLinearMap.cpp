// wuzp-expect: error
// wuzp-static-assert(units=wuzp|mp_units): Units do not match\.
// wuzp-error(units=boost,compiler=gnu): no match for 'operator\+' \(operand types are 'boost::units::unit<
// wuzp-error(units=boost,compiler=clang|appleclang): invalid operands to binary expression \('boost::units::unit<
// wuzp-error(units=boost,compiler=msvc): binary '\+': 'T' does not define this operator

// The units [[m, m], [m, s]] do not describe a linear map, so the terms of the determinant have different units.

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  const wuzp::Quantities<ValueMatrix<double, 2, 2>, wuzp::TypeMatrix<2, 2, Length, Length, Length, Time>> quantities{1.0, 2.0, 3.0, 4.0};
  [[maybe_unused]] const auto determinant = quantities.determinant();
  return 0;
}
