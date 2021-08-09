#pragma once

namespace ctm {
namespace units {

template <int Time, int Length, int Mass, int ElectricCurrent, int ThermodynamicTemperature, int AmountOfSubstance, int LuminousIntensity>
struct Unit {};

using Dimensionless = Unit<0, 0, 0, 0, 0, 0, 0>;
using Angle = Unit<0, 0, 0, 0, 0, 0, 0>;
using Time = Unit<1, 0, 0, 0, 0, 0, 0>;
using Frequency = Unit<-1, 0, 0, 0, 0, 0, 0>;
using AngularVelocity = Unit<-1, 0, 0, 0, 0, 0, 0>;
using AngularAcceleration = Unit<-2, 0, 0, 0, 0, 0, 0>;
using Length = Unit<0, 1, 0, 0, 0, 0, 0>;
using Area = Unit<0, 2, 0, 0, 0, 0, 0>;
using Volume = Unit<0, 3, 0, 0, 0, 0, 0>;
using Velocity = Unit<-1, 1, 0, 0, 0, 0, 0>;
using Acceleration = Unit<-2, 1, 0, 0, 0, 0, 0>;
using Mass = Unit<0, 0, 1, 0, 0, 0, 0>;
using Force = Unit<-2, 1, 1, 0, 0, 0, 0>;
using Energy = Unit<-2, 2, 1, 0, 0, 0, 0>;

// TODO(remod): Is a wrapper the right solution?
template <typename Unit>
struct Absolute {};

}  // namespace units
}  // namespace ctm
