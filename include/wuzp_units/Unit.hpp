#pragma once

#include "wuzp/core/traits.hpp"
#include "wuzp/units/Unit.hpp"

namespace wuzp_units {

template <bool IsAbsolute, int Time, int Length, int Mass, int ElectricCurrent, int ThermodynamicTemperature, int AmountOfSubstance,
          int LuminousIntensity>
struct UnitImpl;

using Acceleration = wuzp::Unit<UnitImpl<false, -2, 1, 0, 0, 0, 0, 0>>;
using Angle = wuzp::Unit<UnitImpl<false, 0, 0, 0, 0, 0, 0, 0>>;
using AngularAcceleration = wuzp::Unit<UnitImpl<false, -2, 0, 0, 0, 0, 0, 0>>;
using AngularVelocity = wuzp::Unit<UnitImpl<false, -1, 0, 0, 0, 0, 0, 0>>;
using Area = wuzp::Unit<UnitImpl<false, 0, 2, 0, 0, 0, 0, 0>>;
using Dimensionless = wuzp::Unit<UnitImpl<false, 0, 0, 0, 0, 0, 0, 0>>;
using Energy = wuzp::Unit<UnitImpl<false, -2, 2, 1, 0, 0, 0, 0>>;
using Force = wuzp::Unit<UnitImpl<false, -2, 1, 1, 0, 0, 0, 0>>;
using Frequency = wuzp::Unit<UnitImpl<false, -1, 0, 0, 0, 0, 0, 0>>;
using Length = wuzp::Unit<UnitImpl<false, 0, 1, 0, 0, 0, 0, 0>>;
using Mass = wuzp::Unit<UnitImpl<false, 0, 0, 1, 0, 0, 0, 0>>;
using Time = wuzp::Unit<UnitImpl<false, 1, 0, 0, 0, 0, 0, 0>>;
using Velocity = wuzp::Unit<UnitImpl<false, -1, 1, 0, 0, 0, 0, 0>>;
using Volume = wuzp::Unit<UnitImpl<false, 0, 3, 0, 0, 0, 0, 0>>;

using AbsoluteAcceleration = wuzp::Unit<UnitImpl<true, -2, 1, 0, 0, 0, 0, 0>>;
using AbsoluteAngle = wuzp::Unit<UnitImpl<true, 0, 0, 0, 0, 0, 0, 0>>;
using AbsoluteAngularAcceleration = wuzp::Unit<UnitImpl<true, -2, 0, 0, 0, 0, 0, 0>>;
using AbsoluteAngularVelocity = wuzp::Unit<UnitImpl<true, -1, 0, 0, 0, 0, 0, 0>>;
using AbsoluteArea = wuzp::Unit<UnitImpl<true, 0, 2, 0, 0, 0, 0, 0>>;
using AbsoluteDimensionless = wuzp::Unit<UnitImpl<true, 0, 0, 0, 0, 0, 0, 0>>;
using AbsoluteEnergy = wuzp::Unit<UnitImpl<true, -2, 2, 1, 0, 0, 0, 0>>;
using AbsoluteForce = wuzp::Unit<UnitImpl<true, -2, 1, 1, 0, 0, 0, 0>>;
using AbsoluteFrequency = wuzp::Unit<UnitImpl<true, -1, 0, 0, 0, 0, 0, 0>>;
using AbsoluteLength = wuzp::Unit<UnitImpl<true, 0, 1, 0, 0, 0, 0, 0>>;
using AbsoluteMass = wuzp::Unit<UnitImpl<true, 0, 0, 1, 0, 0, 0, 0>>;
using AbsoluteTime = wuzp::Unit<UnitImpl<true, 1, 0, 0, 0, 0, 0, 0>>;
using AbsoluteVelocity = wuzp::Unit<UnitImpl<true, -1, 1, 0, 0, 0, 0, 0>>;
using AbsoluteVolume = wuzp::Unit<UnitImpl<true, 0, 3, 0, 0, 0, 0, 0>>;

}  // namespace wuzp_units

namespace wuzp {

template <bool IsAbsoluteLhs, int... ExponentsLhs, bool IsAbsoluteRhs, int... ExponentsRhs>
struct Sum<Unit<wuzp_units::UnitImpl<IsAbsoluteLhs, ExponentsLhs...>>, Unit<wuzp_units::UnitImpl<IsAbsoluteRhs, ExponentsRhs...>>> {
  static_assert(((ExponentsLhs == ExponentsRhs) && ...), "Units do not match.");
  static_assert(!IsAbsoluteLhs || !IsAbsoluteRhs, "Two absolute units cannot be summed.");
  using type = typename Choose<IsAbsoluteLhs, Unit<wuzp_units::UnitImpl<IsAbsoluteLhs, ExponentsLhs...>>,
                               Unit<wuzp_units::UnitImpl<IsAbsoluteRhs, ExponentsRhs...>>>::type;
};

template <bool IsAbsoluteLhs, int... ExponentsLhs, bool IsAbsoluteRhs, int... ExponentsRhs>
struct Difference<Unit<wuzp_units::UnitImpl<IsAbsoluteLhs, ExponentsLhs...>>, Unit<wuzp_units::UnitImpl<IsAbsoluteRhs, ExponentsRhs...>>> {
  static_assert(((ExponentsLhs == ExponentsRhs) && ...), "Units do not match.");
  static_assert(!(!IsAbsoluteLhs && IsAbsoluteRhs), "An absolute unit cannot be subtracted from a relative unit.");
  using type = typename Choose<IsAbsoluteLhs,
                               typename Choose<IsAbsoluteRhs, Unit<wuzp_units::UnitImpl<false, ExponentsLhs...>>,
                                               Unit<wuzp_units::UnitImpl<true, ExponentsLhs...>>>::type,
                               Unit<wuzp_units::UnitImpl<false, ExponentsRhs...>>>::type;
};

template <bool IsAbsoluteLhs, int... ExponentsLhs, bool IsAbsoluteRhs, int... ExponentsRhs>
struct Product<Unit<wuzp_units::UnitImpl<IsAbsoluteLhs, ExponentsLhs...>>, Unit<wuzp_units::UnitImpl<IsAbsoluteRhs, ExponentsRhs...>>> {
  static_assert(!IsAbsoluteLhs && !IsAbsoluteRhs, "An absolute unit cannot appear in a multiplication.");
  using type = Unit<wuzp_units::UnitImpl<false, (ExponentsLhs + ExponentsRhs)...>>;
};

template <bool IsAbsoluteLhs, int... ExponentsLhs, bool IsAbsoluteRhs, int... ExponentsRhs>
struct Division<Unit<wuzp_units::UnitImpl<IsAbsoluteLhs, ExponentsLhs...>>, Unit<wuzp_units::UnitImpl<IsAbsoluteRhs, ExponentsRhs...>>> {
  static_assert(!IsAbsoluteLhs && !IsAbsoluteRhs, "An absolute unit cannot appear in a division.");
  using type = Unit<wuzp_units::UnitImpl<false, (ExponentsLhs - ExponentsRhs)...>>;
};

template <bool IsAbsolute, int... Exponents, int Exponent>
struct Power<Unit<wuzp_units::UnitImpl<IsAbsolute, Exponents...>>, Exponent> {
  static_assert(!IsAbsolute, "An absolute unit cannot be powered.");
  using type = Unit<wuzp_units::UnitImpl<false, (Exponent * Exponents)...>>;
};

template <bool IsAbsolute, int... Exponents>
struct Identity<Unit<wuzp_units::UnitImpl<IsAbsolute, Exponents...>>> {
  using type = Unit<wuzp_units::UnitImpl<false, 0, 0, 0, 0, 0, 0, 0>>;
};

template <bool IsAbsolute, int... Exponents>
struct Inverse<Unit<wuzp_units::UnitImpl<IsAbsolute, Exponents...>>> {
  using type = Unit<wuzp_units::UnitImpl<false, (-Exponents)...>>;
};

template <bool IsAbsoluteV, int... ExponentsV>
struct IsAbsolute<Unit<wuzp_units::UnitImpl<IsAbsoluteV, ExponentsV...>>> {
  static constexpr bool value = IsAbsoluteV;
};

template <bool IsAbsolute, int... Exponents>
struct MakeRelative<Unit<wuzp_units::UnitImpl<IsAbsolute, Exponents...>>> {
  static_assert(IsAbsolute, "Unit is already relative.");
  using type = Unit<wuzp_units::UnitImpl<false, Exponents...>>;
};

template <bool IsAbsolute, int... Exponents>
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
