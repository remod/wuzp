#pragma once

#include <boost/units/absolute.hpp>
#include <boost/units/dimensionless_type.hpp>
#include <boost/units/operators.hpp>
#include <boost/units/static_rational.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/unit.hpp>

#include "wuzp/core/traits.hpp"
#include "wuzp/units/Unit.hpp"

namespace wuzp_boost {

// template <typename Implementation>
// using Unit = wuzp::Unit<Implementation>;

// template <typename Dim, typename System, typename Enable>
// using Unit = wuzp::Unit<boost::units::unit<Dim, System, Enable>>;

// template <typename Unit>
// using Absolute = boost::units::absolute<Unit>;

// These typedefs make sure all unit implementations have the same interface.
// We could use the pure implementation types here, but then everywhere in the code we have to wrap them with Unit<>.

using Angle = wuzp::Unit<boost::units::si::plane_angle>;
using AbsoluteAngle = wuzp::Unit<boost::units::absolute<boost::units::si::plane_angle>>;
using AngularVelocity = wuzp::Unit<boost::units::si::angular_velocity>;
using AbsoluteAngularVelocity = wuzp::Unit<boost::units::absolute<boost::units::si::angular_velocity>>;
using AngularAcceleration = wuzp::Unit<boost::units::si::angular_acceleration>;
using AbsoluteAngularAcceleration = wuzp::Unit<boost::units::absolute<boost::units::si::angular_acceleration>>;
using Length = wuzp::Unit<boost::units::si::length>;
using AbsoluteLength = wuzp::Unit<boost::units::absolute<boost::units::si::length>>;
using Time = wuzp::Unit<boost::units::si::time>;
using AbsoluteTime = wuzp::Unit<boost::units::absolute<boost::units::si::time>>;
using Area = wuzp::Unit<boost::units::si::area>;
using AbsoluteArea = wuzp::Unit<boost::units::absolute<boost::units::si::area>>;
using Mass = wuzp::Unit<boost::units::si::mass>;
using AbsoluteMass = wuzp::Unit<boost::units::absolute<boost::units::si::mass>>;
using Force = wuzp::Unit<boost::units::si::force>;
using AbsoluteForce = wuzp::Unit<boost::units::absolute<boost::units::si::force>>;
using Energy = wuzp::Unit<boost::units::si::energy>;
using AbsoluteEnergy = wuzp::Unit<boost::units::absolute<boost::units::si::energy>>;
using Dimensionless = wuzp::Unit<boost::units::si::dimensionless>;
using AbsoluteDimensionless = wuzp::Unit<boost::units::absolute<boost::units::si::dimensionless>>;
using Velocity = wuzp::Unit<boost::units::si::velocity>;
using AbsoluteVelocity = wuzp::Unit<boost::units::absolute<boost::units::si::velocity>>;
using Acceleration = wuzp::Unit<boost::units::si::acceleration>;
using AbsoluteAcceleration = wuzp::Unit<boost::units::absolute<boost::units::si::acceleration>>;
using Volume = wuzp::Unit<boost::units::si::volume>;
using AbsoluteVolume = wuzp::Unit<boost::units::absolute<boost::units::si::volume>>;
using Frequency = wuzp::Unit<boost::units::si::frequency>;
using AbsoluteFrequency = wuzp::Unit<boost::units::absolute<boost::units::si::frequency>>;

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
  //  using type = Unit;
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

}  // namespace wuzp
