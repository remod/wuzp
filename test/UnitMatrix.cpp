#include <gtest/gtest.h>

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/units/Unit.hpp"
#include "wuzp/units/UnitMatrix.hpp"

using wuzp::Difference;
using wuzp::Inverse;
using wuzp::Product;
using wuzp::Sum;
using namespace wuzp::units;  // NOLINT

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
