#pragma once

#include <type_traits>

#include <mp-units/framework.h>
#include <mp-units/systems/si.h>

#include "wuzp/core/Unit.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp_mp_units {

// The type of an mp-units unit object, e.g. UnitType<mp_units::si::metre / mp_units::si::second>. mp-units gives a unit
// the same type however it is built (m / s and m * m / (s * m) have the same type), so units can be compared by their
// types.
template <auto UnitV_>
using UnitType = std::remove_cvref_t<decltype(UnitV_)>;

// The units are built from the base units rather than named derived units (e.g. kg m / s^2 rather than newton), so that
// they have the same types as the results of products and divisions. Angles are radians, which mp-units keeps apart from
// the unit one (rad / s is not 1 / s), like Boost.Units does, but unlike wuzp's own units.
using Acceleration = wuzp::Unit<UnitType<mp_units::si::metre / mp_units::square(mp_units::si::second)>>;
using Angle = wuzp::Unit<UnitType<mp_units::si::radian>>;
using AngularAcceleration = wuzp::Unit<UnitType<mp_units::si::radian / mp_units::square(mp_units::si::second)>>;
using AngularVelocity = wuzp::Unit<UnitType<mp_units::si::radian / mp_units::si::second>>;
using Area = wuzp::Unit<UnitType<mp_units::square(mp_units::si::metre)>>;
using Dimensionless = wuzp::Unit<UnitType<mp_units::one>>;
using Energy =
    wuzp::Unit<UnitType<mp_units::si::kilogram * mp_units::square(mp_units::si::metre) / mp_units::square(mp_units::si::second)>>;
using Force = wuzp::Unit<UnitType<mp_units::si::kilogram * mp_units::si::metre / mp_units::square(mp_units::si::second)>>;
using Frequency = wuzp::Unit<UnitType<mp_units::one / mp_units::si::second>>;
using Length = wuzp::Unit<UnitType<mp_units::si::metre>>;
using Mass = wuzp::Unit<UnitType<mp_units::si::kilogram>>;
using Time = wuzp::Unit<UnitType<mp_units::si::second>>;
// Torque is N m, like in mp-units' SI system, so that the cross product of a length and a force is a torque.
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

template <mp_units::Unit UnitLhsT_, mp_units::Unit UnitRhsT_>
struct Product<Unit<UnitLhsT_>, Unit<UnitRhsT_>> {
  using type = Unit<std::remove_cvref_t<decltype(UnitLhsT_{} * UnitRhsT_{})>>;
};

template <mp_units::Unit UnitLhsT_, mp_units::Unit UnitRhsT_>
struct Division<Unit<UnitLhsT_>, Unit<UnitRhsT_>> {
  using type = Unit<std::remove_cvref_t<decltype(UnitLhsT_{} / UnitRhsT_{})>>;
};

template <mp_units::Unit UnitT_, typename ExponentT_>
struct Power<Unit<UnitT_>, ExponentT_> {
  using type = Unit<std::remove_cvref_t<decltype(mp_units::pow<ExponentT_::Dividend, ExponentT_::Divisor>(UnitT_{}))>>;
};

template <mp_units::Unit UnitT_>
struct Identity<Unit<UnitT_>> {
  using type = wuzp_mp_units::Dimensionless;
};

template <mp_units::Unit UnitT_>
struct Inverse<Unit<UnitT_>> {
  using type = Unit<std::remove_cvref_t<decltype(mp_units::one / UnitT_{})>>;
};

template <typename T>
struct AccessUnits<AccessImplementation<T>> {
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
