#include <gtest/gtest.h>

#include <boost/units/absolute.hpp>

#include "ctm/boost/boost.hpp"
#include "ctm/core/TypeMatrix.hpp"

namespace units = boost::units::si;
using boost::units::absolute;
using ctm::Difference;
using ctm::Inverse;
using ctm::Product;
using ctm::Sum;
using ctm::UnitMatrix;
using ctm::UnitTraits;

TEST(BoostUnit, Sum) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Sum<units::length, units::length>::type, units::length>::value);
  static_assert(std::is_same<UnitTraits::Sum<absolute<units::length>, units::length>::type, absolute<units::length>>::value);
  static_assert(std::is_same<UnitTraits::Sum<units::length, absolute<units::length>>::type, absolute<units::length>>::value);
}

TEST(BoostUnit, Difference) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Difference<units::length, units::length>::type, units::length>::value);
  static_assert(std::is_same<UnitTraits::Difference<absolute<units::length>, units::length>::type, absolute<units::length>>::value);
}

TEST(BoostUnit, Product) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Product<units::length, units::length>::type, units::area>::value);
}

TEST(BoostUnitMatrix, Division) {  // NOLINT
  //  static_assert(std::is_same<UnitTraits::Division<units::length, units::time>::type, absolute<units::velocity>>::value);
  static_assert(std::is_same<UnitTraits::Division<units::length, units::time>::type, units::velocity>::value);
}

TEST(BoostUnit, Inverse) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Inverse<units::time>::type, units::frequency>::value);
}

TEST(BoostUnit, Identity) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Identity<units::length>::type, units::dimensionless>::value);
  static_assert(std::is_same<UnitTraits::Identity<units::mass>::type, units::dimensionless>::value);
  static_assert(std::is_same<UnitTraits::Identity<units::dimensionless>::type, units::dimensionless>::value);
}

TEST(BoostUnitMatrix, Sum) {  // NOLINT
  static_assert(
      std::is_same<Sum<UnitMatrix<2, 2, units::length>, UnitMatrix<2, 2, units::length>>::type, UnitMatrix<2, 2, units::length>>::value);
  using MyUnits = UnitMatrix<3, 2, units::length, units::area, units::volume, units::mass, units::force, units::energy>;
  static_assert(std::is_same<Sum<MyUnits, MyUnits>::type, MyUnits>::value);
}

TEST(BoostUnitMatrix, Difference) {  // NOLINT
  static_assert(std::is_same<Difference<UnitMatrix<2, 2, units::length>, UnitMatrix<2, 2, units::length>>::type,
                             UnitMatrix<2, 2, units::length>>::value);
  using MyUnits = UnitMatrix<3, 2, units::length, units::area, units::volume, units::mass, units::force, units::energy>;
  static_assert(std::is_same<Difference<MyUnits, MyUnits>::type, MyUnits>::value);
}

TEST(BoostUnitMatrix, Product) {  // NOLINT
  static_assert(
      std::is_same<Product<UnitMatrix<2, 2, units::length>, UnitMatrix<2, 2, units::length>>::type, UnitMatrix<2, 2, units::area>>::value);
}

TEST(BoostUnitMatrix, Inverse) {  // NOLINT
  static_assert(std::is_same<Inverse<UnitMatrix<2, 2, units::time>>::type, UnitMatrix<2, 2, units::frequency>>::value);
}
