#pragma once

#include <ratio>
#include <type_traits>

#include <mp-units/framework.h>
#include <mp-units/systems/si.h>

#include "wuzp/core/Unit.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp_mp_units {
namespace internal {

template <typename... RatiosT_>
struct RatioSum {
  using type = std::ratio<0>;
};
template <typename RatioT_, typename... RatiosT_>
struct RatioSum<RatioT_, RatiosT_...> {
  using type = std::ratio_add<RatioT_, typename RatioSum<RatiosT_...>::type>;
};

using Gram = std::remove_cvref_t<decltype(mp_units::si::gram)>;

//! The exponent of the gram in a unit of mp-units built from base units, e.g. 1 in g m / s^2, as std::ratio (in `type`).
template <typename UnitT_>
struct GramExponent {
  using type = std::ratio<0>;
};
template <>
struct GramExponent<Gram> {
  using type = std::ratio<1>;
};
template <int NumeratorV_, int... DenominatorV_>
struct GramExponent<mp_units::power<Gram, NumeratorV_, DenominatorV_...>> {
  using type = std::ratio<NumeratorV_, DenominatorV_...>;
};
template <typename FactorT_, typename... FactorsT_>
struct GramExponent<mp_units::per<FactorT_, FactorsT_...>> {
  using type =
      std::ratio_subtract<std::ratio<0>,
                          typename RatioSum<typename GramExponent<FactorT_>::type, typename GramExponent<FactorsT_>::type...>::type>;
};
template <typename... FactorsT_>
struct GramExponent<mp_units::derived_unit<FactorsT_...>> {
  using type = typename RatioSum<typename GramExponent<FactorsT_>::type...>::type;
};

//! The coherent SI unit with a reference unit of mp-units (see Normalize): the gram replaced by the kilogram.
template <typename ReferenceT_>
consteval auto coherentUnit(ReferenceT_ reference) {
  using GramExponentT = typename GramExponent<ReferenceT_>::type;
  // mp_units::pow only takes positive exponents for named units, so negative ones divide (std::ratio keeps den positive).
  constexpr auto num = GramExponentT::num > 0 ? GramExponentT::num : -GramExponentT::num;
  if constexpr (GramExponentT::num > 0) {
    return mp_units::pow<num, GramExponentT::den>(mp_units::si::kilogram) *
           (reference / mp_units::pow<num, GramExponentT::den>(mp_units::si::gram));
  } else if constexpr (GramExponentT::num < 0) {
    return reference * mp_units::pow<num, GramExponentT::den>(mp_units::si::gram) /
           mp_units::pow<num, GramExponentT::den>(mp_units::si::kilogram);
  } else {
    return reference;
  }
}

/**
 * The unit of wuzp for a unit of mp-units: the coherent SI unit built from the base units, e.g. kg m / s^2 for si::newton.
 * mp-units gives a unit the same type however it is built from other units (m / s and m * m / (s * m) have the same type), but
 * named units have their own types, e.g. si::newton is not the type of kg m / s^2 (see "Many shades of the same unit" and "Named
 * units do not simplify in unit equations" in https://mpusz.github.io/mp-units/latest/users_guide/framework_basics/systems_of_units/).
 * wuzp compares units by their types, so it normalizes them.
 *
 * The canonical unit of mp-units is a reference unit built from the base units, with the gram rather than the kilogram, and a
 * magnitude (see get_canonical_unit() and derived_unit in mp-units' framework/unit.h). The coherent SI unit has the same
 * reference unit with the kilogram instead of the gram. Units with another magnitude, e.g. km, g or deg, are rejected: wuzp does
 * not convert units.
 */
template <typename UnitT_>
struct Normalize {
 private:
  static constexpr auto canonical = mp_units::get_canonical_unit(UnitT_{});
  static constexpr auto coherent = coherentUnit(canonical.reference_unit);
  static_assert(mp_units::get_canonical_unit(coherent).mag == canonical.mag,
                "Only coherent SI units are supported, e.g. m, kg and rad but not km, g or deg: wuzp does not convert units.");

 public:
  using type = std::remove_cvref_t<decltype(coherent)>;
};

}  // namespace internal

// The type of an mp-units unit object, e.g. UnitType<mp_units::si::metre / mp_units::si::second> or
// UnitType<mp_units::si::newton>, normalized to the coherent SI unit built from the base units (see internal::Normalize), so
// that equal units have the same type. Units that are not coherent SI units, e.g. km, are rejected.
template <auto UnitV_>
using UnitType = typename internal::Normalize<std::remove_cvref_t<decltype(UnitV_)>>::type;

// Angles are dimensionless in every units backend (see "Angles are dimensionless" in DESIGN.md), so they are the unit one
// rather than mp-units' radian, and angular velocity is 1 / s.
using Acceleration = wuzp::Unit<UnitType<mp_units::si::metre / mp_units::square(mp_units::si::second)>>;
using Angle = wuzp::Unit<UnitType<mp_units::one>>;
using AngularAcceleration = wuzp::Unit<UnitType<mp_units::one / mp_units::square(mp_units::si::second)>>;
using AngularVelocity = wuzp::Unit<UnitType<mp_units::one / mp_units::si::second>>;
using Area = wuzp::Unit<UnitType<mp_units::square(mp_units::si::metre)>>;
using Dimensionless = wuzp::Unit<UnitType<mp_units::one>>;
using Energy =
    wuzp::Unit<UnitType<mp_units::si::kilogram * mp_units::square(mp_units::si::metre) / mp_units::square(mp_units::si::second)>>;
using Force = wuzp::Unit<UnitType<mp_units::si::kilogram * mp_units::si::metre / mp_units::square(mp_units::si::second)>>;
using Frequency = wuzp::Unit<UnitType<mp_units::one / mp_units::si::second>>;
using Length = wuzp::Unit<UnitType<mp_units::si::metre>>;
using Mass = wuzp::Unit<UnitType<mp_units::si::kilogram>>;
using Time = wuzp::Unit<UnitType<mp_units::si::second>>;
// Torque is N m, the unit of energy, so that the cross product of a length and a force is a torque.
using Torque =
    wuzp::Unit<UnitType<mp_units::si::kilogram * mp_units::square(mp_units::si::metre) / mp_units::square(mp_units::si::second)>>;
using Velocity = wuzp::Unit<UnitType<mp_units::si::metre / mp_units::si::second>>;
using Volume = wuzp::Unit<UnitType<mp_units::cubic(mp_units::si::metre)>>;

}  // namespace wuzp_mp_units

namespace wuzp {

template <mp_units::Unit UnitLhsT_, mp_units::Unit UnitRhsT_>
struct Sum<Unit<UnitLhsT_>, Unit<UnitRhsT_>> {
  static_assert(std::is_same_v<UnitLhsT_, UnitRhsT_>, "Units do not match.");
  using type = Unit<UnitLhsT_>;
};

template <mp_units::Unit UnitLhsT_, mp_units::Unit UnitRhsT_>
struct Difference<Unit<UnitLhsT_>, Unit<UnitRhsT_>> {
  static_assert(std::is_same_v<UnitLhsT_, UnitRhsT_>, "Units do not match.");
  using type = Unit<UnitLhsT_>;
};

// The results of unit arithmetic are normalized like the units themselves, see wuzp_mp_units::UnitType.
template <mp_units::Unit UnitLhsT_, mp_units::Unit UnitRhsT_>
struct Product<Unit<UnitLhsT_>, Unit<UnitRhsT_>> {
  using type = Unit<wuzp_mp_units::UnitType<UnitLhsT_{} * UnitRhsT_{}>>;
};

template <mp_units::Unit UnitLhsT_, mp_units::Unit UnitRhsT_>
struct Division<Unit<UnitLhsT_>, Unit<UnitRhsT_>> {
  using type = Unit<wuzp_mp_units::UnitType<UnitLhsT_{} / UnitRhsT_{}>>;
};

template <mp_units::Unit UnitT_, typename ExponentT_>
struct Power<Unit<UnitT_>, ExponentT_> {
  using type = Unit<wuzp_mp_units::UnitType<mp_units::pow<ExponentT_::Dividend, ExponentT_::Divisor>(UnitT_{})>>;
};

template <mp_units::Unit UnitT_>
struct Identity<Unit<UnitT_>> {
  using type = wuzp_mp_units::Dimensionless;
};

template <mp_units::Unit UnitT_>
struct Inverse<Unit<UnitT_>> {
  using type = Unit<wuzp_mp_units::UnitType<mp_units::one / UnitT_{}>>;
};

template <typename ValueT_>
struct AccessUnits<AccessImplementation<ValueT_>> {
  using Acceleration = wuzp_mp_units::Acceleration;
  using Angle = wuzp_mp_units::Angle;
  using AngularAcceleration = wuzp_mp_units::AngularAcceleration;
  using AngularVelocity = wuzp_mp_units::AngularVelocity;
  using Area = wuzp_mp_units::Area;
  using Dimensionless = wuzp_mp_units::Dimensionless;
  using Energy = wuzp_mp_units::Energy;
  using Force = wuzp_mp_units::Force;
  using Frequency = wuzp_mp_units::Frequency;
  using Length = wuzp_mp_units::Length;
  using Mass = wuzp_mp_units::Mass;
  using Time = wuzp_mp_units::Time;
  using Torque = wuzp_mp_units::Torque;
  using Velocity = wuzp_mp_units::Velocity;
  using Volume = wuzp_mp_units::Volume;
};

}  // namespace wuzp
