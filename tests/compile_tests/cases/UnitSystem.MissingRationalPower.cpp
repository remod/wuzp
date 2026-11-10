// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The core raises units to rational powers, e.g. for pow<1, 2>(), so UnitSystem requires them. The units archetype without the
// square root of an area, like a units backend whose Power only takes integer exponents, does not satisfy it: it misses only
// this requirement.

#include "wuzp/core/Rational.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

#include "archetype_units/Unit.hpp"

namespace wuzp {

// The power of the archetype, but without the type of the square root of an area.
template <>
struct Power<archetype_units::Area, Rational<1, 2>> {};

}  // namespace wuzp

namespace {

template <typename UnitsT_>
  requires wuzp::UnitSystem<UnitsT_>
struct CheckedUnits {};

}  // namespace

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(CheckedUnits<wuzp::AccessUnits<wuzp::AccessImplementation<double>>>);
  return 0;
}
