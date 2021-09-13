#pragma once

#include <boost/units/absolute.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/unit.hpp>

namespace wuzp {

template <typename Dim, typename System, typename Enable>
using Unit = boost::units::unit<Dim, System, Enable>;

template <typename Unit>
using Absolute = boost::units::absolute<Unit>;

using Angle = boost::units::si::plane_angle;
using AngularVelocity = boost::units::si::angular_velocity;
using AngularAcceleration = boost::units::si::angular_acceleration;
using Length = boost::units::si::length;
using Time = boost::units::si::time;
using Area = boost::units::si::area;
using Mass = boost::units::si::mass;
using Force = boost::units::si::force;
using Energy = boost::units::si::energy;
using Dimensionless = boost::units::si::dimensionless;
using Velocity = boost::units::si::velocity;
using Acceleration = boost::units::si::acceleration;
using Volume = boost::units::si::volume;
using Frequency = boost::units::si::frequency;

}  // namespace wuzp
