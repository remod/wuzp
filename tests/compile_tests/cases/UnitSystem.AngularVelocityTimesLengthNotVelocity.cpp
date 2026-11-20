// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The transport formulas of rigid body kinematics, e.g. v + w x r, need the product of an angular velocity and a length to be a
// velocity, so UnitSystem requires it (#76). The units archetype with a wrong product, like a units backend whose angles are not
// dimensionless, does not satisfy it: it misses only this requirement.

#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

#include "archetype_units/Unit.hpp"

namespace wuzp {

// The product of the archetype, but an angular velocity times a length is a length.
template <>
struct Product<archetype_units::AngularVelocity, archetype_units::Length> {
  using type = archetype_units::Length;
};

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
