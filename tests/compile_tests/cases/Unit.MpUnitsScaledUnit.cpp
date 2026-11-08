// wuzp-expect: error
// wuzp-requires: units=mp_units
// wuzp-static-assert: Only coherent SI units are supported, e\.g\. m, kg and rad but not km, g or deg: wuzp does not convert units\.

// wuzp has no unit conversions, so the mp-units adapter rejects units that are not coherent SI units, e.g. km (#65).

#include "wuzp_mp_units/Unit.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::Unit<wuzp_mp_units::UnitType<mp_units::si::kilo<mp_units::si::metre>>>);
  return 0;
}
