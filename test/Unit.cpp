#include <gtest/gtest.h>

#include "ctm/core/TypeMatrix.hpp"
#include "ctm/units/Unit.hpp"
#include "ctm/units/UnitMatrix.hpp"
#include "ctm/units/UnitTraits.hpp"

using ctm::Difference;
using ctm::Inverse;
using ctm::Product;
using ctm::Sum;
using namespace ctm::units;  // NOLINT

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

TEST(TestUnitMatrix, Division) {  // NOLINT
  //  static_assert(std::is_same<UnitTraits::Division<Length, Time>::type, Absolute<units::velocity>>::value);
  static_assert(std::is_same<UnitTraits::Division<Length, Time>::type, Velocity>::value);
}

TEST(TestUnit, Inverse) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Inverse<Time>::type, Frequency>::value);
}

TEST(TestUnit, Identity) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Identity<Length>::type, Dimensionless>::value);
  static_assert(std::is_same<UnitTraits::Identity<Mass>::type, Dimensionless>::value);
  static_assert(std::is_same<UnitTraits::Identity<Dimensionless>::type, Dimensionless>::value);
}

TEST(TestUnitMatrix, Sum) {  // NOLINT
  static_assert(std::is_same<Sum<UnitMatrix<2, 2, Length>, UnitMatrix<2, 2, Length>>::type, UnitMatrix<2, 2, Length>>::value);
  using MyUnits = UnitMatrix<3, 2, Length, Area, Volume, Mass, Force, Energy>;
  static_assert(std::is_same<Sum<MyUnits, MyUnits>::type, MyUnits>::value);
}

TEST(TestUnitMatrix, Difference) {  // NOLINT
  static_assert(std::is_same<Difference<UnitMatrix<2, 2, Length>, UnitMatrix<2, 2, Length>>::type, UnitMatrix<2, 2, Length>>::value);
  using MyUnits = UnitMatrix<3, 2, Length, Area, Volume, Mass, Force, Energy>;
  static_assert(std::is_same<Difference<MyUnits, MyUnits>::type, MyUnits>::value);
}

TEST(TestUnitMatrix, Product) {  // NOLINT
  static_assert(std::is_same<Product<UnitMatrix<2, 2, Length>, UnitMatrix<2, 2, Length>>::type, UnitMatrix<2, 2, Area>>::value);
}

TEST(TestUnitMatrix, Inverse) {  // NOLINT
  static_assert(std::is_same<Inverse<UnitMatrix<2, 2, Time>>::type, UnitMatrix<2, 2, Frequency>>::value);
}

// TEST(TestUnitMatrix, Test) {  // NOLINT
//  using boost::units::quantity;
//  quantity<units::plane_angle> x;
//  x = 3.*units::radian;
//  x = 3.*units::si_dimensionless;
//  x = 3.*units::radian;
////  quantity<units::plane_angle> y = 3.*units::kilogram;
//}
