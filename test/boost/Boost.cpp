#include <gtest/gtest.h>

#include <boost/units/absolute.hpp>

#include "ctm/boost/boost.hpp"
#include "ctm/core/TypeMatrix.hpp"

using boost::units::si::area;
using boost::units::si::dimensionless;
using boost::units::si::energy;
using boost::units::si::force;
using boost::units::si::frequency;
using boost::units::si::length;
using boost::units::si::mass;
using boost::units::si::volume;
using ctm::BoostTraits;
using ctm::Difference;
using ctm::Inverse;
using ctm::Product;
using ctm::Sum;
using ctm::UnitMatrix;

TEST(BoostUnit, Sum) {  // NOLINT
  static_assert(std::is_same<BoostTraits::Sum<length, length>::type, length>::value);
  static_assert(std::is_same<BoostTraits::Sum<boost::units::absolute<length>, length>::type, boost::units::absolute<length>>::value);
  static_assert(std::is_same<BoostTraits::Sum<length, boost::units::absolute<length>>::type, boost::units::absolute<length>>::value);
}

TEST(BoostUnit, Difference) {  // NOLINT
  static_assert(std::is_same<BoostTraits::Difference<length, length>::type, length>::value);
  static_assert(std::is_same<BoostTraits::Difference<boost::units::absolute<length>, length>::type, boost::units::absolute<length>>::value);
}

TEST(BoostUnit, Product) {  // NOLINT
  static_assert(std::is_same<BoostTraits::Product<length, length>::type, area>::value);
}

TEST(BoostUnit, Inverse) {  // NOLINT
  static_assert(std::is_same<BoostTraits::Inverse<boost::units::si::time>::type, frequency>::value);
}

TEST(BoostUnit, Identity) {  // NOLINT
  static_assert(std::is_same<BoostTraits::Identity<length>::type, dimensionless>::value);
  static_assert(std::is_same<BoostTraits::Identity<mass>::type, dimensionless>::value);
  static_assert(std::is_same<BoostTraits::Identity<dimensionless>::type, dimensionless>::value);
}

TEST(BoostUnitMatrix, Sum) {  // NOLINT
  static_assert(std::is_same<Sum<UnitMatrix<2, 2, length>, UnitMatrix<2, 2, length>>::type, UnitMatrix<2, 2, length>>::value);
  using MyUnits = UnitMatrix<3, 2, length, area, volume, mass, force, energy>;
  static_assert(std::is_same<Sum<MyUnits, MyUnits>::type, MyUnits>::value);
}

TEST(BoostUnitMatrix, Difference) {  // NOLINT
  static_assert(std::is_same<Difference<UnitMatrix<2, 2, length>, UnitMatrix<2, 2, length>>::type, UnitMatrix<2, 2, length>>::value);
  using MyUnits = UnitMatrix<3, 2, length, area, volume, mass, force, energy>;
  static_assert(std::is_same<Difference<MyUnits, MyUnits>::type, MyUnits>::value);
}

TEST(BoostUnitMatrix, Product) {  // NOLINT
  static_assert(std::is_same<Product<UnitMatrix<2, 2, length>, UnitMatrix<2, 2, length>>::type, UnitMatrix<2, 2, area>>::value);
}

TEST(BoostUnitMatrix, Inverse) {  // NOLINT
  static_assert(std::is_same<Inverse<UnitMatrix<2, 2, boost::units::si::time>>::type, UnitMatrix<2, 2, frequency>>::value);
}
