#pragma once

#include "wuzp/core/Unit.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

// The units archetype: a minimal units backend, which provides exactly what the concept UnitSystem in wuzp/core/concepts.hpp
// requires, and nothing more, like the archetypes of Boost's Concept Check library
// (https://www.boost.org/doc/libs/release/libs/concept_check/doc/reference.htm#basic-archetype). The presets
// <compiler>-archetype-archetype run the whole test suite against it, which checks that the core uses nothing beyond the
// interface of the units backends. It is test code: it is neither installed nor part of the public headers.
//
// It specializes the arithmetic traits Product, Division, Power, Inverse and Identity for its units, and AccessUnits for every
// arithmetic value type. The core defines the sum and the difference of units, which only exist for equal units (see Unit.hpp).

namespace archetype_units {

//! A unit, given by the integer exponents of the only base dimensions that the units of UnitSystem need: time, length and mass.
template <int TimeV_, int LengthV_, int MassV_>
struct Exponents {};

namespace internal {

//! The exponent of a base dimension raised to the power RationalT_, Rational<Dividend, Divisor>, and whether it is an integer.
template <int ExponentV_, typename RationalT_>
inline constexpr bool hasIntegerPower = (ExponentV_ * RationalT_::Dividend) % RationalT_::Divisor == 0;
template <int ExponentV_, typename RationalT_>
inline constexpr int power = ExponentV_ * RationalT_::Dividend / RationalT_::Divisor;

}  // namespace internal

// Angles are dimensionless (see "Angles are dimensionless" in DESIGN.md).
using Acceleration = wuzp::Unit<Exponents<-2, 1, 0>>;
using Angle = wuzp::Unit<Exponents<0, 0, 0>>;
using AngularAcceleration = wuzp::Unit<Exponents<-2, 0, 0>>;
using AngularVelocity = wuzp::Unit<Exponents<-1, 0, 0>>;
using Area = wuzp::Unit<Exponents<0, 2, 0>>;
using Dimensionless = wuzp::Unit<Exponents<0, 0, 0>>;
using Energy = wuzp::Unit<Exponents<-2, 2, 1>>;
using Force = wuzp::Unit<Exponents<-2, 1, 1>>;
using Frequency = wuzp::Unit<Exponents<-1, 0, 0>>;
using Length = wuzp::Unit<Exponents<0, 1, 0>>;
using Mass = wuzp::Unit<Exponents<0, 0, 1>>;
using Time = wuzp::Unit<Exponents<1, 0, 0>>;
using Torque = wuzp::Unit<Exponents<-2, 2, 1>>;
using Velocity = wuzp::Unit<Exponents<-1, 1, 0>>;
using Volume = wuzp::Unit<Exponents<0, 3, 0>>;

}  // namespace archetype_units

namespace wuzp {

template <int TimeLhsV_, int LengthLhsV_, int MassLhsV_, int TimeRhsV_, int LengthRhsV_, int MassRhsV_>
struct Product<Unit<archetype_units::Exponents<TimeLhsV_, LengthLhsV_, MassLhsV_>>,
               Unit<archetype_units::Exponents<TimeRhsV_, LengthRhsV_, MassRhsV_>>> {
  using type = Unit<archetype_units::Exponents<TimeLhsV_ + TimeRhsV_, LengthLhsV_ + LengthRhsV_, MassLhsV_ + MassRhsV_>>;
};

template <int TimeLhsV_, int LengthLhsV_, int MassLhsV_, int TimeRhsV_, int LengthRhsV_, int MassRhsV_>
struct Division<Unit<archetype_units::Exponents<TimeLhsV_, LengthLhsV_, MassLhsV_>>,
                Unit<archetype_units::Exponents<TimeRhsV_, LengthRhsV_, MassRhsV_>>> {
  using type = Unit<archetype_units::Exponents<TimeLhsV_ - TimeRhsV_, LengthLhsV_ - LengthRhsV_, MassLhsV_ - MassRhsV_>>;
};

// The power with the rational exponent ExponentT_, Rational<Dividend, Divisor>, only if the exponents of the result are integers,
// e.g. the square root of an area, but not of a length.
template <int TimeV_, int LengthV_, int MassV_, typename ExponentT_>
  requires(archetype_units::internal::hasIntegerPower<TimeV_, ExponentT_> &&
           archetype_units::internal::hasIntegerPower<LengthV_, ExponentT_> &&
           archetype_units::internal::hasIntegerPower<MassV_, ExponentT_>)
struct Power<Unit<archetype_units::Exponents<TimeV_, LengthV_, MassV_>>, ExponentT_> {
  using type = Unit<archetype_units::Exponents<archetype_units::internal::power<TimeV_, ExponentT_>,
                                               archetype_units::internal::power<LengthV_, ExponentT_>,
                                               archetype_units::internal::power<MassV_, ExponentT_>>>;
};

template <int TimeV_, int LengthV_, int MassV_>
struct Identity<Unit<archetype_units::Exponents<TimeV_, LengthV_, MassV_>>> {
  using type = archetype_units::Dimensionless;
};

template <int TimeV_, int LengthV_, int MassV_>
struct Inverse<Unit<archetype_units::Exponents<TimeV_, LengthV_, MassV_>>> {
  using type = Unit<archetype_units::Exponents<-TimeV_, -LengthV_, -MassV_>>;
};

template <ArithmeticValue ValueT_>
struct AccessUnits<AccessImplementation<ValueT_>> {
  using Acceleration = archetype_units::Acceleration;
  using Angle = archetype_units::Angle;
  using AngularAcceleration = archetype_units::AngularAcceleration;
  using AngularVelocity = archetype_units::AngularVelocity;
  using Area = archetype_units::Area;
  using Dimensionless = archetype_units::Dimensionless;
  using Energy = archetype_units::Energy;
  using Force = archetype_units::Force;
  using Frequency = archetype_units::Frequency;
  using Length = archetype_units::Length;
  using Mass = archetype_units::Mass;
  using Time = archetype_units::Time;
  using Torque = archetype_units::Torque;
  using Velocity = archetype_units::Velocity;
  using Volume = archetype_units::Volume;
};

}  // namespace wuzp
