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

// Angles are dimensionless in every units backend (see "Angles are dimensionless" in DESIGN.md), unlike Boost.Units'
// plane_angle: angular velocity is 1 / s, and torque is N m, the unit of energy.
using Acceleration = wuzp::Unit<boost::units::si::acceleration>;
using Angle = wuzp::Unit<boost::units::si::dimensionless>;
using AngularAcceleration = wuzp::Unit<boost::units::divide_typeof_helper<boost::units::si::frequency, boost::units::si::time>::type>;
using AngularVelocity = wuzp::Unit<boost::units::si::frequency>;
using Area = wuzp::Unit<boost::units::si::area>;
using Dimensionless = wuzp::Unit<boost::units::si::dimensionless>;
using Energy = wuzp::Unit<boost::units::si::energy>;
using Force = wuzp::Unit<boost::units::si::force>;
using Torque = wuzp::Unit<boost::units::si::energy>;
using Frequency = wuzp::Unit<boost::units::si::frequency>;
using Length = wuzp::Unit<boost::units::si::length>;
using Mass = wuzp::Unit<boost::units::si::mass>;
using Time = wuzp::Unit<boost::units::si::time>;
using Velocity = wuzp::Unit<boost::units::si::velocity>;
using Volume = wuzp::Unit<boost::units::si::volume>;

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

template <typename Implementation, typename Exponent>
struct Power<Unit<Implementation>, Exponent> {
  using type = Unit<typename boost::units::power_typeof_helper<Implementation,
                                                               boost::units::static_rational<Exponent::Dividend, Exponent::Divisor>>::type>;
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

template <typename T>
struct AccessUnits<AccessImplementation<T>> {
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
  using Torque = wuzp_boost::Torque;
  using Velocity = wuzp_boost::Velocity;
  using Volume = wuzp_boost::Volume;
};

}  // namespace wuzp
