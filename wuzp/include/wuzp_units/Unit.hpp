#pragma once

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/Rational.hpp"
#include "wuzp/core/Unit.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp_units {

template <typename Time, typename Length, typename Mass, typename ElectricCurrent, typename ThermodynamicTemperature,
          typename AmountOfSubstance, typename LuminousIntensity>
struct UnitImpl;

template <int Time, int Length, int Mass, int ElectricCurrent, int ThermodynamicTemperature, int AmountOfSubstance, int LuminousIntensity>
using CreateIntegerUnitImpl =
    UnitImpl<wuzp::Integer<Time>, wuzp::Integer<Length>, wuzp::Integer<Mass>, wuzp::Integer<ElectricCurrent>,
             wuzp::Integer<ThermodynamicTemperature>, wuzp::Integer<AmountOfSubstance>, wuzp::Integer<LuminousIntensity>>;

using Acceleration = wuzp::Unit<CreateIntegerUnitImpl<-2, 1, 0, 0, 0, 0, 0>>;
using Angle = wuzp::Unit<CreateIntegerUnitImpl<0, 0, 0, 0, 0, 0, 0>>;
using AngularAcceleration = wuzp::Unit<CreateIntegerUnitImpl<-2, 0, 0, 0, 0, 0, 0>>;
using AngularVelocity = wuzp::Unit<CreateIntegerUnitImpl<-1, 0, 0, 0, 0, 0, 0>>;
using Area = wuzp::Unit<CreateIntegerUnitImpl<0, 2, 0, 0, 0, 0, 0>>;
using Dimensionless = wuzp::Unit<CreateIntegerUnitImpl<0, 0, 0, 0, 0, 0, 0>>;
using Energy = wuzp::Unit<CreateIntegerUnitImpl<-2, 2, 1, 0, 0, 0, 0>>;
using Force = wuzp::Unit<CreateIntegerUnitImpl<-2, 1, 1, 0, 0, 0, 0>>;
using Frequency = wuzp::Unit<CreateIntegerUnitImpl<-1, 0, 0, 0, 0, 0, 0>>;
using Length = wuzp::Unit<CreateIntegerUnitImpl<0, 1, 0, 0, 0, 0, 0>>;
using Mass = wuzp::Unit<CreateIntegerUnitImpl<0, 0, 1, 0, 0, 0, 0>>;
using Time = wuzp::Unit<CreateIntegerUnitImpl<1, 0, 0, 0, 0, 0, 0>>;
using Velocity = wuzp::Unit<CreateIntegerUnitImpl<-1, 1, 0, 0, 0, 0, 0>>;
using Volume = wuzp::Unit<CreateIntegerUnitImpl<0, 3, 0, 0, 0, 0, 0>>;

}  // namespace wuzp_units

namespace wuzp {

template <typename... ExponentsLhs, typename... ExponentsRhs>
struct Sum<Unit<wuzp_units::UnitImpl<ExponentsLhs...>>, Unit<wuzp_units::UnitImpl<ExponentsRhs...>>> {
  static_assert(std::is_same_v<std::tuple<ExponentsLhs...>, std::tuple<ExponentsRhs...>>, "Units do not match.");
  using type = Unit<wuzp_units::UnitImpl<ExponentsLhs...>>;
};

template <typename... ExponentsLhs, typename... ExponentsRhs>
struct Difference<Unit<wuzp_units::UnitImpl<ExponentsLhs...>>, Unit<wuzp_units::UnitImpl<ExponentsRhs...>>> {
  static_assert(std::is_same_v<std::tuple<ExponentsLhs...>, std::tuple<ExponentsRhs...>>, "Units do not match.");
  using type = Unit<wuzp_units::UnitImpl<ExponentsLhs...>>;
};

template <typename... ExponentsLhs, typename... ExponentsRhs>
struct Product<Unit<wuzp_units::UnitImpl<ExponentsLhs...>>, Unit<wuzp_units::UnitImpl<ExponentsRhs...>>> {
  using type = Unit<wuzp_units::UnitImpl<typename Sum<ExponentsLhs, ExponentsRhs>::type...>>;
};

template <typename... ExponentsLhs, typename... ExponentsRhs>
struct Division<Unit<wuzp_units::UnitImpl<ExponentsLhs...>>, Unit<wuzp_units::UnitImpl<ExponentsRhs...>>> {
  using type = Unit<wuzp_units::UnitImpl<typename Difference<ExponentsLhs, ExponentsRhs>::type...>>;
};

template <typename... ExponentsT, typename ExponentT>
struct Power<Unit<wuzp_units::UnitImpl<ExponentsT...>>, ExponentT> {
  using type = Unit<wuzp_units::UnitImpl<typename Product<ExponentT, ExponentsT>::type...>>;
};

template <typename... Exponents>
struct Identity<Unit<wuzp_units::UnitImpl<Exponents...>>> {
  using type = Unit<wuzp_units::CreateIntegerUnitImpl<0, 0, 0, 0, 0, 0, 0>>;
};

template <typename... Exponents>
struct Inverse<Unit<wuzp_units::UnitImpl<Exponents...>>> {
  using type = Unit<wuzp_units::UnitImpl<typename Negate<Exponents>::type...>>;
};

template <typename T>
struct AccessUnits<AccessImplementation<T>> {
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
  using Velocity = wuzp_units::Velocity;
  using Volume = wuzp_units::Volume;
};

}  // namespace wuzp
