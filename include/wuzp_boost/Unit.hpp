#pragma once

#include <boost/units/absolute.hpp>
#include <boost/units/dimensionless_type.hpp>
#include <boost/units/operators.hpp>
#include <boost/units/static_rational.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/unit.hpp>

#include "wuzp/core/Unit.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp_boost {

using Acceleration = wuzp::Unit<boost::units::si::acceleration>;
using Angle = wuzp::Unit<boost::units::si::plane_angle>;
using AngularAcceleration = wuzp::Unit<boost::units::si::angular_acceleration>;
using AngularVelocity = wuzp::Unit<boost::units::si::angular_velocity>;
using Area = wuzp::Unit<boost::units::si::area>;
using Dimensionless = wuzp::Unit<boost::units::si::dimensionless>;
using Energy = wuzp::Unit<boost::units::si::energy>;
using Force = wuzp::Unit<boost::units::si::force>;
using Frequency = wuzp::Unit<boost::units::si::frequency>;
using Length = wuzp::Unit<boost::units::si::length>;
using Mass = wuzp::Unit<boost::units::si::mass>;
using Time = wuzp::Unit<boost::units::si::time>;
using Velocity = wuzp::Unit<boost::units::si::velocity>;
using Volume = wuzp::Unit<boost::units::si::volume>;

using AbsoluteAcceleration = wuzp::Unit<boost::units::absolute<boost::units::si::acceleration>>;
using AbsoluteAngle = wuzp::Unit<boost::units::absolute<boost::units::si::plane_angle>>;
using AbsoluteAngularAcceleration = wuzp::Unit<boost::units::absolute<boost::units::si::angular_acceleration>>;
using AbsoluteAngularVelocity = wuzp::Unit<boost::units::absolute<boost::units::si::angular_velocity>>;
using AbsoluteArea = wuzp::Unit<boost::units::absolute<boost::units::si::area>>;
using AbsoluteDimensionless = wuzp::Unit<boost::units::absolute<boost::units::si::dimensionless>>;
using AbsoluteEnergy = wuzp::Unit<boost::units::absolute<boost::units::si::energy>>;
using AbsoluteForce = wuzp::Unit<boost::units::absolute<boost::units::si::force>>;
using AbsoluteFrequency = wuzp::Unit<boost::units::absolute<boost::units::si::frequency>>;
using AbsoluteLength = wuzp::Unit<boost::units::absolute<boost::units::si::length>>;
using AbsoluteMass = wuzp::Unit<boost::units::absolute<boost::units::si::mass>>;
using AbsoluteTime = wuzp::Unit<boost::units::absolute<boost::units::si::time>>;
using AbsoluteVelocity = wuzp::Unit<boost::units::absolute<boost::units::si::velocity>>;
using AbsoluteVolume = wuzp::Unit<boost::units::absolute<boost::units::si::volume>>;

}  // namespace wuzp_boost

namespace wuzp {

template <typename ImplementationLhs, typename ImplementationRhs>
struct Sum<Unit<ImplementationLhs>, Unit<ImplementationRhs>> {
  using type = Unit<typename boost::units::add_typeof_helper<ImplementationLhs, ImplementationRhs>::type>;
};

template <typename ImplementationLhs, typename ImplementationRhs>
struct Difference<Unit<ImplementationLhs>, Unit<ImplementationRhs>> {
  using type = Unit<typename boost::units::subtract_typeof_helper<ImplementationLhs, ImplementationRhs>::type>;
};

template <typename ImplementationLhs, typename ImplementationRhs>
struct Product<Unit<ImplementationLhs>, Unit<ImplementationRhs>> {
  using type = Unit<typename boost::units::multiply_typeof_helper<ImplementationLhs, ImplementationRhs>::type>;
};

template <typename ImplementationLhs, typename ImplementationRhs>
struct Division<Unit<ImplementationLhs>, Unit<ImplementationRhs>> {
  using type = Unit<typename boost::units::divide_typeof_helper<ImplementationLhs, ImplementationRhs>::type>;
};

template <typename Implementation, int Exponent>
struct Power<Unit<Implementation>, Exponent> {
  using type = Unit<typename boost::units::power_typeof_helper<Implementation, boost::units::static_rational<Exponent>>::type>;
};

template <typename Dim, typename System>
struct Identity<Unit<boost::units::unit<Dim, System>>> {
  using type = Unit<boost::units::unit<boost::units::dimensionless_type, System>>;
};
template <typename Dim, typename System>
struct Identity<Unit<boost::units::absolute<boost::units::unit<Dim, System>>>> {
  using type = Unit<boost::units::absolute<boost::units::unit<boost::units::dimensionless_type, System>>>;
};

template <typename Implementation>
struct Inverse<Unit<Implementation>> {
  using type = Unit<typename boost::units::power_typeof_helper<Implementation, boost::units::static_rational<-1>>::type>;
};

template <typename Implementation>
struct IsAbsolute<Unit<Implementation>> : public std::false_type {};
template <typename Implementation>
struct IsAbsolute<Unit<boost::units::absolute<Implementation>>> : public std::true_type {};

template <typename Implementation>
struct MakeRelative<Unit<Implementation>> {
  static_assert(FailOnInstantiation<MakeRelative<Unit<Implementation>>>::value, "Unit is already relative.");
};
template <typename Implementation>
struct MakeRelative<Unit<boost::units::absolute<Implementation>>> {
  using type = Unit<Implementation>;
};

template <typename Implementation>
struct MakeAbsolute<Unit<Implementation>> {
  using type = Unit<boost::units::absolute<Implementation>>;
};
template <typename Implementation>
struct MakeAbsolute<Unit<boost::units::absolute<Implementation>>> {
  static_assert(FailOnInstantiation<Unit<boost::units::absolute<Implementation>>>::value, "Unit is already absolute.");
};

template <typename T>
struct AccessUnits<SpecializationDummy<T>> {
  using AbsoluteAcceleration = wuzp_boost::AbsoluteAcceleration;
  using AbsoluteAngle = wuzp_boost::AbsoluteAngle;
  using AbsoluteAngularAcceleration = wuzp_boost::AbsoluteAngularAcceleration;
  using AbsoluteAngularVelocity = wuzp_boost::AbsoluteAngularVelocity;
  using AbsoluteArea = wuzp_boost::AbsoluteArea;
  using AbsoluteDimensionless = wuzp_boost::AbsoluteDimensionless;
  using AbsoluteEnergy = wuzp_boost::AbsoluteEnergy;
  using AbsoluteForce = wuzp_boost::AbsoluteForce;
  using AbsoluteFrequency = wuzp_boost::AbsoluteFrequency;
  using AbsoluteLength = wuzp_boost::AbsoluteLength;
  using AbsoluteMass = wuzp_boost::AbsoluteMass;
  using AbsoluteTime = wuzp_boost::AbsoluteTime;
  using AbsoluteVelocity = wuzp_boost::AbsoluteVelocity;
  using AbsoluteVolume = wuzp_boost::AbsoluteVolume;

  using Acceleration = wuzp_boost::Acceleration;
  using Angle = wuzp_boost::Angle;
  using AngularAcceleration = wuzp_boost::AngularAcceleration;
  using AngularVelocity = wuzp_boost::AngularVelocity;
  using Area = wuzp_boost::Area;
  using Dimensionless = wuzp_boost::Dimensionless;
  using Energy = wuzp_boost::Energy;
  using Force = wuzp_boost::Force;
  using Frequency = wuzp_boost::Frequency;
  using Length = wuzp_boost::Length;
  using Mass = wuzp_boost::Mass;
  using Time = wuzp_boost::Time;
  using Velocity = wuzp_boost::Velocity;
  using Volume = wuzp_boost::Volume;
};

}  // namespace wuzp
