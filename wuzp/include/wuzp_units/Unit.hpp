#pragma once

#include <tuple>
#include <type_traits>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/Rational.hpp"
#include "wuzp/core/Unit.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp_units {

//! A unit, given by the rational exponents of the seven SI base units.
template <typename TimeT_, typename LengthT_, typename MassT_, typename ElectricCurrentT_, typename ThermodynamicTemperatureT_,
          typename AmountOfSubstanceT_, typename LuminousIntensityT_>
struct UnitImpl;

template <int TimeV_, int LengthV_, int MassV_, int ElectricCurrentV_, int ThermodynamicTemperatureV_, int AmountOfSubstanceV_,
          int LuminousIntensityV_>
using CreateIntegerUnitImpl =
    UnitImpl<wuzp::Integer<TimeV_>, wuzp::Integer<LengthV_>, wuzp::Integer<MassV_>, wuzp::Integer<ElectricCurrentV_>,
             wuzp::Integer<ThermodynamicTemperatureV_>, wuzp::Integer<AmountOfSubstanceV_>, wuzp::Integer<LuminousIntensityV_>>;

using Acceleration = wuzp::Unit<CreateIntegerUnitImpl<-2, 1, 0, 0, 0, 0, 0>>;
using Angle = wuzp::Unit<CreateIntegerUnitImpl<0, 0, 0, 0, 0, 0, 0>>;
using AngularAcceleration = wuzp::Unit<CreateIntegerUnitImpl<-2, 0, 0, 0, 0, 0, 0>>;
using AngularVelocity = wuzp::Unit<CreateIntegerUnitImpl<-1, 0, 0, 0, 0, 0, 0>>;
using Area = wuzp::Unit<CreateIntegerUnitImpl<0, 2, 0, 0, 0, 0, 0>>;
using Dimensionless = wuzp::Unit<CreateIntegerUnitImpl<0, 0, 0, 0, 0, 0, 0>>;
using Energy = wuzp::Unit<CreateIntegerUnitImpl<-2, 2, 1, 0, 0, 0, 0>>;
using Force = wuzp::Unit<CreateIntegerUnitImpl<-2, 1, 1, 0, 0, 0, 0>>;
using Torque = wuzp::Unit<CreateIntegerUnitImpl<-2, 2, 1, 0, 0, 0, 0>>;
using Frequency = wuzp::Unit<CreateIntegerUnitImpl<-1, 0, 0, 0, 0, 0, 0>>;
using Length = wuzp::Unit<CreateIntegerUnitImpl<0, 1, 0, 0, 0, 0, 0>>;
using Mass = wuzp::Unit<CreateIntegerUnitImpl<0, 0, 1, 0, 0, 0, 0>>;
using Time = wuzp::Unit<CreateIntegerUnitImpl<1, 0, 0, 0, 0, 0, 0>>;
using Velocity = wuzp::Unit<CreateIntegerUnitImpl<-1, 1, 0, 0, 0, 0, 0>>;
using Volume = wuzp::Unit<CreateIntegerUnitImpl<0, 3, 0, 0, 0, 0, 0>>;

}  // namespace wuzp_units

namespace wuzp {

template <typename... ExponentsLhsT_, typename... ExponentsRhsT_>
struct Sum<Unit<wuzp_units::UnitImpl<ExponentsLhsT_...>>, Unit<wuzp_units::UnitImpl<ExponentsRhsT_...>>> {
  static_assert(std::is_same_v<std::tuple<ExponentsLhsT_...>, std::tuple<ExponentsRhsT_...>>, "Units do not match.");
  using type = Unit<wuzp_units::UnitImpl<ExponentsLhsT_...>>;
};

template <typename... ExponentsLhsT_, typename... ExponentsRhsT_>
struct Difference<Unit<wuzp_units::UnitImpl<ExponentsLhsT_...>>, Unit<wuzp_units::UnitImpl<ExponentsRhsT_...>>> {
  static_assert(std::is_same_v<std::tuple<ExponentsLhsT_...>, std::tuple<ExponentsRhsT_...>>, "Units do not match.");
  using type = Unit<wuzp_units::UnitImpl<ExponentsLhsT_...>>;
};

template <typename... ExponentsLhsT_, typename... ExponentsRhsT_>
struct Product<Unit<wuzp_units::UnitImpl<ExponentsLhsT_...>>, Unit<wuzp_units::UnitImpl<ExponentsRhsT_...>>> {
  using type = Unit<wuzp_units::UnitImpl<typename Sum<ExponentsLhsT_, ExponentsRhsT_>::type...>>;
};

template <typename... ExponentsLhsT_, typename... ExponentsRhsT_>
struct Division<Unit<wuzp_units::UnitImpl<ExponentsLhsT_...>>, Unit<wuzp_units::UnitImpl<ExponentsRhsT_...>>> {
  using type = Unit<wuzp_units::UnitImpl<typename Difference<ExponentsLhsT_, ExponentsRhsT_>::type...>>;
};

template <typename... ExponentsT_, typename ExponentT_>
struct Power<Unit<wuzp_units::UnitImpl<ExponentsT_...>>, ExponentT_> {
  using type = Unit<wuzp_units::UnitImpl<typename Product<ExponentT_, ExponentsT_>::type...>>;
};

template <typename... ExponentsT_>
struct Identity<Unit<wuzp_units::UnitImpl<ExponentsT_...>>> {
  using type = Unit<wuzp_units::CreateIntegerUnitImpl<0, 0, 0, 0, 0, 0, 0>>;
};

template <typename... ExponentsT_>
struct Inverse<Unit<wuzp_units::UnitImpl<ExponentsT_...>>> {
  using type = Unit<wuzp_units::UnitImpl<typename Negation<ExponentsT_>::type...>>;
};

template <typename ValueT_>
struct AccessUnits<AccessImplementation<ValueT_>> {
  using Acceleration = wuzp_units::Acceleration;
  using Angle = wuzp_units::Angle;
  using AngularAcceleration = wuzp_units::AngularAcceleration;
  using AngularVelocity = wuzp_units::AngularVelocity;
  using Area = wuzp_units::Area;
  using Dimensionless = wuzp_units::Dimensionless;
  using Energy = wuzp_units::Energy;
  using Force = wuzp_units::Force;
  using Frequency = wuzp_units::Frequency;
  using Length = wuzp_units::Length;
  using Mass = wuzp_units::Mass;
  using Time = wuzp_units::Time;
  using Torque = wuzp_units::Torque;
  using Velocity = wuzp_units::Velocity;
  using Volume = wuzp_units::Volume;
};

}  // namespace wuzp
