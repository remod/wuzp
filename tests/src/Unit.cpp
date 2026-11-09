#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

#include "Unit.hpp"

using namespace wuzp;  // NOLINT

// The units of the units backend are consistent. The backends do not check this in their headers, see UnitSystem.
static_assert(UnitSystem<AccessUnits<AccessImplementation<double>>>);

namespace {

// The units of the units backend, except that torque is a force rather than the unit of energy.
struct InconsistentUnits : AccessUnits<AccessImplementation<double>> {
  using Torque = Force;
};

// No units at all.
struct MissingUnits {};

// The units of the units backend, except that the length is a value type rather than a unit.
struct LengthNotAUnit : AccessUnits<AccessImplementation<double>> {
  using Length = double;
};

}  // namespace

static_assert(!UnitSystem<InconsistentUnits>);
static_assert(!UnitSystem<MissingUnits>);
static_assert(!UnitSystem<LengthNotAUnit>);

TEST(TestUnit, Sum) {  // NOLINT
  static_assert(std::is_same<Sum<Length, Length>::type, Length>::value);
}

TEST(TestUnit, Difference) {  // NOLINT
  static_assert(std::is_same<Difference<Length, Length>::type, Length>::value);
}

TEST(TestUnit, Product) {  // NOLINT
  static_assert(std::is_same<Product<Length, Length>::type, Area>::value);
}

TEST(TestUnit, Division) {  // NOLINT
  static_assert(std::is_same<Division<Length, Time>::type, Velocity>::value);
}

TEST(TestUnit, Power) {  // NOLINT
  static_assert(std::is_same<Power<Length, Integer<3>>::type, Volume>::value);
}

TEST(TestUnit, Inverse) {  // NOLINT
  static_assert(std::is_same<Inverse<Time>::type, Frequency>::value);
}

TEST(TestUnit, Identity) {  // NOLINT
  static_assert(std::is_same<Identity<Length>::type, Dimensionless>::value);
  static_assert(std::is_same<Identity<Mass>::type, Dimensionless>::value);
  static_assert(std::is_same<Identity<Dimensionless>::type, Dimensionless>::value);
}

TEST(TestUnit, AreAnglesDimensionless) {  // NOLINT
  // Angles are dimensionless with every units backend (see "Angles are dimensionless" in DESIGN.md).
  static_assert(std::is_same<Angle, Dimensionless>::value);
  static_assert(std::is_same<Product<Angle, Length>::type, Length>::value);
  static_assert(std::is_same<Product<AngularVelocity, Length>::type, Velocity>::value);
  static_assert(std::is_same<Product<AngularAcceleration, Length>::type, Acceleration>::value);
  static_assert(std::is_same<Torque, Energy>::value);
}

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_MP_UNITS
// mp-units gives named units their own types, e.g. si::newton is not the type of kg m / s^2. The adapter normalizes them to
// the coherent SI units built from the base units, which wuzp computes (#65). The other units backends have no such named
// units: Boost.Units gives equal units the same type.
TEST(TestUnit, NamedUnitsOfMpUnits) {  // NOLINT
  static_assert(std::is_same<wuzp::Unit<UnitType<mp_units::si::newton>>, Force>::value);
  static_assert(std::is_same<wuzp::Unit<UnitType<mp_units::si::joule>>, Energy>::value);
  static_assert(std::is_same<wuzp::Unit<UnitType<mp_units::si::hertz>>, Frequency>::value);
  static_assert(std::is_same<wuzp::Unit<UnitType<mp_units::si::radian>>, Angle>::value);
  static_assert(std::is_same<wuzp::Unit<UnitType<mp_units::si::kilogram>>, Mass>::value);
  static_assert(std::is_same<Product<Mass, Acceleration>::type, wuzp::Unit<UnitType<mp_units::si::newton>>>::value);
  static_assert(std::is_same<Product<Force, Length>::type, wuzp::Unit<UnitType<mp_units::si::joule>>>::value);
  static_assert(std::is_same<Division<wuzp::Unit<UnitType<mp_units::si::watt>>, Force>::type, Velocity>::value);
}
#endif
