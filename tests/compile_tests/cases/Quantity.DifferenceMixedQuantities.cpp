// wuzp-expect: error
// wuzp-static-assert(units=wuzp): Units do not match\.
// wuzp-error(units=boost,compiler=gnu): no match for 'operator-' \(operand types are 'boost::units::unit<
// wuzp-error(units=boost,compiler=clang|appleclang): invalid operands to binary expression \('boost::units::unit<
// wuzp-error(units=boost,compiler=msvc): binary '-': 'T' does not define this operator

#include "wuzp/core/Quantities.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

using TimeQuantity = QuantityD<Time>;
using LengthQuantity = QuantityD<Length>;

int main() {
  [[maybe_unused]] const auto invalid = TimeQuantity{2.0} - LengthQuantity{3.0};
  return 0;
}
