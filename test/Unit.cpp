#include <gtest/gtest.h>

#include "wuzp/units/Unit.hpp"
#include "wuzp/units/UnitTraits.hpp"

using namespace wuzp::units;  // NOLINT

TEST(TestUnit, Sum) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Sum<Length, Length>::type, Length>::value);
  static_assert(std::is_same<UnitTraits::Sum<Absolute<Length>, Length>::type, Absolute<Length>>::value);
  static_assert(std::is_same<UnitTraits::Sum<Length, Absolute<Length>>::type, Absolute<Length>>::value);
}

TEST(TestUnit, Difference) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Difference<Length, Length>::type, Length>::value);
  static_assert(std::is_same<UnitTraits::Difference<Absolute<Length>, Length>::type, Absolute<Length>>::value);
}

TEST(TestUnit, Product) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Product<Length, Length>::type, Area>::value);
}

TEST(TestUnit, Division) {  // NOLINT
  // TODO(remod): Absolute or relative?
  //  static_assert(std::is_same<UnitTraits::Division<Length, Time>::type, Absolute<units::velocity>>::value);
  static_assert(std::is_same<UnitTraits::Division<Length, Time>::type, Velocity>::value);
}

TEST(TestUnit, Power) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Power<Length, 3>::type, Volume>::value);
}

TEST(TestUnit, Inverse) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Inverse<Time>::type, Frequency>::value);
}

TEST(TestUnit, Identity) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Identity<Length>::type, Dimensionless>::value);
  static_assert(std::is_same<UnitTraits::Identity<Mass>::type, Dimensionless>::value);
  static_assert(std::is_same<UnitTraits::Identity<Dimensionless>::type, Dimensionless>::value);
}
