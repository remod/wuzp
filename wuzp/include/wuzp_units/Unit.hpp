#pragma once

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/Rational.hpp"
#include "wuzp/core/Unit.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp_units {

template <bool IsAbsolute, typename Time, typename Length, typename Mass, typename ElectricCurrent, typename ThermodynamicTemperature,
          typename AmountOfSubstance, typename LuminousIntensity>
struct UnitImpl;

template <bool IsAbsolute, int Time, int Length, int Mass, int ElectricCurrent, int ThermodynamicTemperature, int AmountOfSubstance,
          int LuminousIntensity>
using CreateIntegerUnitImpl =
    UnitImpl<IsAbsolute, wuzp::Integer<Time>, wuzp::Integer<Length>, wuzp::Integer<Mass>, wuzp::Integer<ElectricCurrent>,
             wuzp::Integer<ThermodynamicTemperature>, wuzp::Integer<AmountOfSubstance>, wuzp::Integer<LuminousIntensity>>;

using Acceleration = wuzp::Unit<CreateIntegerUnitImpl<false, -2, 1, 0, 0, 0, 0, 0>>;
using Angle = wuzp::Unit<CreateIntegerUnitImpl<false, 0, 0, 0, 0, 0, 0, 0>>;
using AngularAcceleration = wuzp::Unit<CreateIntegerUnitImpl<false, -2, 0, 0, 0, 0, 0, 0>>;
using AngularVelocity = wuzp::Unit<CreateIntegerUnitImpl<false, -1, 0, 0, 0, 0, 0, 0>>;
using Area = wuzp::Unit<CreateIntegerUnitImpl<false, 0, 2, 0, 0, 0, 0, 0>>;
using Dimensionless = wuzp::Unit<CreateIntegerUnitImpl<false, 0, 0, 0, 0, 0, 0, 0>>;
using Energy = wuzp::Unit<CreateIntegerUnitImpl<false, -2, 2, 1, 0, 0, 0, 0>>;
using Force = wuzp::Unit<CreateIntegerUnitImpl<false, -2, 1, 1, 0, 0, 0, 0>>;
using Frequency = wuzp::Unit<CreateIntegerUnitImpl<false, -1, 0, 0, 0, 0, 0, 0>>;
using Length = wuzp::Unit<CreateIntegerUnitImpl<false, 0, 1, 0, 0, 0, 0, 0>>;
using Mass = wuzp::Unit<CreateIntegerUnitImpl<false, 0, 0, 1, 0, 0, 0, 0>>;
using Time = wuzp::Unit<CreateIntegerUnitImpl<false, 1, 0, 0, 0, 0, 0, 0>>;
using Velocity = wuzp::Unit<CreateIntegerUnitImpl<false, -1, 1, 0, 0, 0, 0, 0>>;
using Volume = wuzp::Unit<CreateIntegerUnitImpl<false, 0, 3, 0, 0, 0, 0, 0>>;

using AbsoluteAcceleration = wuzp::Unit<CreateIntegerUnitImpl<true, -2, 1, 0, 0, 0, 0, 0>>;
using AbsoluteAngle = wuzp::Unit<CreateIntegerUnitImpl<true, 0, 0, 0, 0, 0, 0, 0>>;
using AbsoluteAngularAcceleration = wuzp::Unit<CreateIntegerUnitImpl<true, -2, 0, 0, 0, 0, 0, 0>>;
using AbsoluteAngularVelocity = wuzp::Unit<CreateIntegerUnitImpl<true, -1, 0, 0, 0, 0, 0, 0>>;
using AbsoluteArea = wuzp::Unit<CreateIntegerUnitImpl<true, 0, 2, 0, 0, 0, 0, 0>>;
using AbsoluteDimensionless = wuzp::Unit<CreateIntegerUnitImpl<true, 0, 0, 0, 0, 0, 0, 0>>;
using AbsoluteEnergy = wuzp::Unit<CreateIntegerUnitImpl<true, -2, 2, 1, 0, 0, 0, 0>>;
using AbsoluteForce = wuzp::Unit<CreateIntegerUnitImpl<true, -2, 1, 1, 0, 0, 0, 0>>;
using AbsoluteFrequency = wuzp::Unit<CreateIntegerUnitImpl<true, -1, 0, 0, 0, 0, 0, 0>>;
using AbsoluteLength = wuzp::Unit<CreateIntegerUnitImpl<true, 0, 1, 0, 0, 0, 0, 0>>;
using AbsoluteMass = wuzp::Unit<CreateIntegerUnitImpl<true, 0, 0, 1, 0, 0, 0, 0>>;
using AbsoluteTime = wuzp::Unit<CreateIntegerUnitImpl<true, 1, 0, 0, 0, 0, 0, 0>>;
using AbsoluteVelocity = wuzp::Unit<CreateIntegerUnitImpl<true, -1, 1, 0, 0, 0, 0, 0>>;
using AbsoluteVolume = wuzp::Unit<CreateIntegerUnitImpl<true, 0, 3, 0, 0, 0, 0, 0>>;

}  // namespace wuzp_units

namespace wuzp {

template <bool IsAbsoluteLhs, typename... ExponentsLhs, bool IsAbsoluteRhs, typename... ExponentsRhs>
struct Sum<Unit<wuzp_units::UnitImpl<IsAbsoluteLhs, ExponentsLhs...>>, Unit<wuzp_units::UnitImpl<IsAbsoluteRhs, ExponentsRhs...>>> {
  static_assert(std::is_same_v<std::tuple<ExponentsLhs...>, std::tuple<ExponentsRhs...>>, "Units do not match.");
  static_assert(!IsAbsoluteLhs || !IsAbsoluteRhs, "Two absolute units cannot be summed.");
  using type = typename IfThenElse<IsAbsoluteLhs, Unit<wuzp_units::UnitImpl<IsAbsoluteLhs, ExponentsLhs...>>,
                                   Unit<wuzp_units::UnitImpl<IsAbsoluteRhs, ExponentsRhs...>>>::type;
};

template <bool IsAbsoluteLhs, typename... ExponentsLhs, bool IsAbsoluteRhs, typename... ExponentsRhs>
struct Difference<Unit<wuzp_units::UnitImpl<IsAbsoluteLhs, ExponentsLhs...>>, Unit<wuzp_units::UnitImpl<IsAbsoluteRhs, ExponentsRhs...>>> {
  static_assert(std::is_same_v<std::tuple<ExponentsLhs...>, std::tuple<ExponentsRhs...>>, "Units do not match.");
  static_assert(!(!IsAbsoluteLhs && IsAbsoluteRhs), "An absolute unit cannot be subtracted from a relative unit.");
  using type = typename IfThenElse<IsAbsoluteLhs,
                                   typename IfThenElse<IsAbsoluteRhs, Unit<wuzp_units::UnitImpl<false, ExponentsLhs...>>,
                                                       Unit<wuzp_units::UnitImpl<true, ExponentsLhs...>>>::type,
                                   Unit<wuzp_units::UnitImpl<false, ExponentsRhs...>>>::type;
};

template <bool IsAbsoluteLhs, typename... ExponentsLhs, bool IsAbsoluteRhs, typename... ExponentsRhs>
struct Product<Unit<wuzp_units::UnitImpl<IsAbsoluteLhs, ExponentsLhs...>>, Unit<wuzp_units::UnitImpl<IsAbsoluteRhs, ExponentsRhs...>>> {
  static_assert(!IsAbsoluteLhs && !IsAbsoluteRhs, "An absolute unit cannot appear in a multiplication.");
  using type = Unit<wuzp_units::UnitImpl<false, typename Sum<ExponentsLhs, ExponentsRhs>::type...>>;
};

template <bool IsAbsoluteLhs, typename... ExponentsLhs, bool IsAbsoluteRhs, typename... ExponentsRhs>
struct Division<Unit<wuzp_units::UnitImpl<IsAbsoluteLhs, ExponentsLhs...>>, Unit<wuzp_units::UnitImpl<IsAbsoluteRhs, ExponentsRhs...>>> {
  static_assert(!IsAbsoluteLhs && !IsAbsoluteRhs, "An absolute unit cannot appear in a division.");
  using type = Unit<wuzp_units::UnitImpl<false, typename Difference<ExponentsLhs, ExponentsRhs>::type...>>;
};

template <bool IsAbsolute, typename... ExponentsT, typename ExponentT>
struct Power<Unit<wuzp_units::UnitImpl<IsAbsolute, ExponentsT...>>, ExponentT> {
  static_assert(!IsAbsolute, "An absolute unit cannot be powered.");
  using type = Unit<wuzp_units::UnitImpl<false, typename Product<ExponentT, ExponentsT>::type...>>;
};

template <bool IsAbsolute, typename... Exponents>
struct Identity<Unit<wuzp_units::UnitImpl<IsAbsolute, Exponents...>>> {
  using type = Unit<wuzp_units::CreateIntegerUnitImpl<false, 0, 0, 0, 0, 0, 0, 0>>;
};

template <bool IsAbsolute, typename... Exponents>
struct Inverse<Unit<wuzp_units::UnitImpl<IsAbsolute, Exponents...>>> {
  using type = Unit<wuzp_units::UnitImpl<false, typename Negate<Exponents>::type...>>;
};

template <bool IsAbsoluteV, typename... ExponentsT>
struct IsAbsolute<Unit<wuzp_units::UnitImpl<IsAbsoluteV, ExponentsT...>>> {
  static constexpr bool value = IsAbsoluteV;
};

template <bool IsAbsolute, typename... Exponents>
struct MakeRelative<Unit<wuzp_units::UnitImpl<IsAbsolute, Exponents...>>> {
  static_assert(IsAbsolute, "Unit is already relative.");
  using type = Unit<wuzp_units::UnitImpl<false, Exponents...>>;
};

template <bool IsAbsolute, typename... Exponents>
struct MakeAbsolute<Unit<wuzp_units::UnitImpl<IsAbsolute, Exponents...>>> {
  static_assert(!IsAbsolute, "Unit is already absolute.");
  using type = Unit<wuzp_units::UnitImpl<true, Exponents...>>;
};

template <typename T>
struct AccessUnits<SpecializationDummy<T>> {
  using AbsoluteAcceleration = wuzp_units::AbsoluteAcceleration;
  using AbsoluteAngle = wuzp_units::AbsoluteAngle;
  using AbsoluteAngularAcceleration = wuzp_units::AbsoluteAngularAcceleration;
  using AbsoluteAngularVelocity = wuzp_units::AbsoluteAngularVelocity;
  using AbsoluteArea = wuzp_units::AbsoluteArea;
  using AbsoluteDimensionless = wuzp_units::AbsoluteDimensionless;
  using AbsoluteEnergy = wuzp_units::AbsoluteEnergy;
  using AbsoluteForce = wuzp_units::AbsoluteForce;
  using AbsoluteFrequency = wuzp_units::AbsoluteFrequency;
  using AbsoluteLength = wuzp_units::AbsoluteLength;
  using AbsoluteMass = wuzp_units::AbsoluteMass;
  using AbsoluteTime = wuzp_units::AbsoluteTime;
  using AbsoluteVelocity = wuzp_units::AbsoluteVelocity;
  using AbsoluteVolume = wuzp_units::AbsoluteVolume;

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
