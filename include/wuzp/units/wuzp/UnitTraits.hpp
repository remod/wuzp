#pragma once

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

template <int Time, int Length, int Mass, int ElectricCurrent, int ThermodynamicTemperature, int AmountOfSubstance, int LuminousIntensity>
struct Unit;
template <typename Unit>
struct Absolute;

struct UnitTraits {
  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename Lhs, typename Rhs>
  struct Sum<Lhs, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Units do not match");
    using type = Lhs;
  };
  template <typename Lhs, typename Rhs>
  struct Sum<Absolute<Lhs>, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Units do not match");
    using type = Absolute<Lhs>;
  };
  template <typename Lhs, typename Rhs>
  struct Sum<Lhs, Absolute<Rhs>> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Units do not match");
    using type = typename Sum<Absolute<Lhs>, Rhs>::type;
  };
  template <typename Lhs, typename Rhs>
  struct Sum<Absolute<Lhs>, Absolute<Rhs>> {
    static_assert(FakeDependency<Sum<Absolute<Lhs>, Absolute<Rhs>>>::value, "Two absolute units cannot be summed.");
  };

  DEFINE_RECURSIVE_TRAIT(Difference)
  template <typename Lhs, typename Rhs>
  struct Difference<Lhs, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Units do not match");
    using type = Lhs;
  };
  template <typename Lhs, typename Rhs>
  struct Difference<Absolute<Lhs>, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Units do not match");
    using type = Absolute<Lhs>;
  };
  template <typename Lhs, typename Rhs>
  struct Difference<Lhs, Absolute<Rhs>> {
    static_assert(FakeDependency<Difference<Lhs, Absolute<Rhs>>>::value, "An absolute unit cannot be subtracted from a relative unit.");
  };
  template <typename Lhs, typename Rhs>
  struct Difference<Absolute<Lhs>, Absolute<Rhs>> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Units do not match");
    using type = Lhs;
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
  //  template <typename Lhs, typename Rhs>
  //  struct Product<Absolute<Lhs>, Rhs> {
  //    using type = Absolute<typename Product<Lhs, Rhs>::type>;
  //  };
  //  template <typename Lhs, typename Rhs>
  //  struct Product<Lhs, Absolute<Rhs>> {
  //    using type = typename Product<Absolute<Rhs>, Lhs>::type;
  //  };

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

  template <typename Unit, int Exponent>
  struct Power;
  template <int Time, int Length, int Mass, int ElectricCurrent, int ThermodynamicTemperature, int AmountOfSubstance, int LuminousIntensity,
            int Exponent>
  struct Power<Unit<Time, Length, Mass, ElectricCurrent, ThermodynamicTemperature, AmountOfSubstance, LuminousIntensity>, Exponent> {
    using type = Unit<Exponent * Time, Exponent * Length, Exponent * Mass, Exponent * ElectricCurrent, Exponent * ThermodynamicTemperature,
                      Exponent * AmountOfSubstance, Exponent * LuminousIntensity>;
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

  template <typename Unit>
  struct IsAbsolute : public std::false_type {};
  template <typename Unit>
  struct IsAbsolute<Absolute<Unit>> : public std::true_type {};

  template <typename Unit>
  struct MakeRelative {
    using type = Unit;
  };
  template <typename Unit>
  struct MakeRelative<Absolute<Unit>> {
    using type = Unit;
  };
  template <typename Unit>
  struct MakeAbsolute {
    using type = Absolute<Unit>;
  };
  template <typename Unit>
  struct MakeAbsolute<Absolute<Unit>> {
    static_assert(FakeDependency<MakeAbsolute<Absolute<Unit>>>::value, "Unit is already absolute.");
  };
};

}  // namespace wuzp
