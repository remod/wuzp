#pragma once

#include "ctm/core/traits.hpp"

namespace ctm {
namespace units {

template <int Time, int Length, int Mass, int ElectricCurrent, int ThermodynamicTemperature, int AmountOfSubstance, int LuminousIntensity>
struct Unit;
template <typename Unit>
struct Absolute;

struct UnitTraits {
  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename Lhs, typename Rhs>
  struct Sum<Lhs, Rhs> {
    using type = Lhs;
  };
  template <typename Lhs, typename Rhs>
  struct Sum<Absolute<Lhs>, Rhs> {
    using type = Absolute<Lhs>;
  };
  template <typename Lhs, typename Rhs>
  struct Sum<Lhs, Absolute<Rhs>> {
    using type = typename Sum<Absolute<Lhs>, Rhs>::type;
  };

  DEFINE_RECURSIVE_TRAIT(Difference)
  template <typename Lhs, typename Rhs>
  struct Difference<Lhs, Rhs> {
    using type = Lhs;
  };
  template <typename Lhs, typename Rhs>
  struct Difference<Absolute<Lhs>, Rhs> {
    using type = Absolute<Lhs>;
  };

  DEFINE_RECURSIVE_TRAIT(Product)
  template <int TimeLhs, int LengthLhs, int MassLhs, int ElectricCurrentLhs, int ThermodynamicTemperatureLhs, int AmountOfSubstanceLhs,
            int LuminousIntensityLhs, int TimeRhs, int LengthRhs, int MassRhs, int ElectricCurrentRhs, int ThermodynamicTemperatureRhs,
            int AmountOfSubstanceRhs, int LuminousIntensityRhs>
  struct Product<
      Unit<TimeLhs, LengthLhs, MassLhs, ElectricCurrentLhs, ThermodynamicTemperatureLhs, AmountOfSubstanceLhs, LuminousIntensityLhs>,
      Unit<TimeRhs, LengthRhs, MassRhs, ElectricCurrentRhs, ThermodynamicTemperatureRhs, AmountOfSubstanceRhs, LuminousIntensityRhs>> {
    using type = Unit<TimeLhs + TimeRhs, LengthLhs + LengthRhs, MassLhs + MassRhs, ElectricCurrentLhs + ElectricCurrentRhs,
                      ThermodynamicTemperatureLhs + ThermodynamicTemperatureRhs, AmountOfSubstanceLhs + AmountOfSubstanceRhs,
                      LuminousIntensityLhs + LuminousIntensityRhs>;
  };

  DEFINE_RECURSIVE_TRAIT(Division)
  template <int TimeLhs, int LengthLhs, int MassLhs, int ElectricCurrentLhs, int ThermodynamicTemperatureLhs, int AmountOfSubstanceLhs,
            int LuminousIntensityLhs, int TimeRhs, int LengthRhs, int MassRhs, int ElectricCurrentRhs, int ThermodynamicTemperatureRhs,
            int AmountOfSubstanceRhs, int LuminousIntensityRhs>
  struct Division<
      Unit<TimeLhs, LengthLhs, MassLhs, ElectricCurrentLhs, ThermodynamicTemperatureLhs, AmountOfSubstanceLhs, LuminousIntensityLhs>,
      Unit<TimeRhs, LengthRhs, MassRhs, ElectricCurrentRhs, ThermodynamicTemperatureRhs, AmountOfSubstanceRhs, LuminousIntensityRhs>> {
    using type = Unit<TimeLhs - TimeRhs, LengthLhs - LengthRhs, MassLhs - MassRhs, ElectricCurrentLhs - ElectricCurrentRhs,
                      ThermodynamicTemperatureLhs - ThermodynamicTemperatureRhs, AmountOfSubstanceLhs - AmountOfSubstanceRhs,
                      LuminousIntensityLhs - LuminousIntensityRhs>;
  };

  template <typename Unit>
  struct Identity;
  template <int Time, int Length, int Mass, int ElectricCurrent, int ThermodynamicTemperature, int AmountOfSubstance, int LuminousIntensity>
  struct Identity<Unit<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature, AmountOfSubstance, LuminousIntensity>> {
    using type = Unit<0, 0, 0, 0, 0, 0, 0>;
  };

  template <typename Unit>
  struct Inverse;
  template <int Time, int Length, int Mass, int ElectricCurrent, int ThermodynamicTemperature, int AmountOfSubstance, int LuminousIntensity>
  struct Inverse<Unit<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature, AmountOfSubstance, LuminousIntensity>> {
    using type = Unit<-Time, -Length, -Mass, -ElectricCurrent, -ThermodynamicTemperature, -AmountOfSubstance, -LuminousIntensity>;
  };
};

}  // namespace units
}  // namespace ctm
