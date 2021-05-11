#include <gtest/gtest.h>

#include "ctm/TypeMatrix.hpp"
#include "ctm/boost/boost.hpp"

using boost::units::si::area;
using boost::units::si::dimensionless;
using boost::units::si::energy;
using boost::units::si::force;
using boost::units::si::frequency;
using boost::units::si::length;
using boost::units::si::mass;
using boost::units::si::volume;
using ctm::Difference;
using ctm::Identity;
using ctm::Inverse;
using ctm::Product;
using ctm::Sum;
using ctm::TypeMatrix;

TEST(BoostUnit, Sum) {  // NOLINT
  static_assert(std::is_same<Sum<length, length>::type, length>::value);
  static_assert(std::is_same<Sum<boost::units::absolute<length>, length>::type, boost::units::absolute<length>>::value);
  static_assert(std::is_same<Sum<length, boost::units::absolute<length>>::type, boost::units::absolute<length>>::value);
}

TEST(BoostUnit, Difference) {  // NOLINT
  static_assert(std::is_same<Difference<length, length>::type, length>::value);
  static_assert(std::is_same<Difference<boost::units::absolute<length>, length>::type, boost::units::absolute<length>>::value);
}

TEST(BoostUnit, Product) {  // NOLINT
  static_assert(std::is_same<Product<length, length>::type, area>::value);
}

TEST(BoostUnit, Inverse) {  // NOLINT
  static_assert(std::is_same<Inverse<boost::units::si::time>::type, frequency>::value);
}

TEST(BoostUnit, Identity) {  // NOLINT
  static_assert(std::is_same<Identity<length>::type, dimensionless>::value);
  static_assert(std::is_same<Identity<mass>::type, dimensionless>::value);
  static_assert(std::is_same<Identity<dimensionless>::type, dimensionless>::value);
}

TEST(BoostUnitMatrix, Sum) {  // NOLINT
  static_assert(std::is_same<Sum<TypeMatrix<2, 2, length>, TypeMatrix<2, 2, length>>::type, TypeMatrix<2, 2, length>>::value);
  using MyUnits = TypeMatrix<3, 2, length, area, volume, mass, force, energy>;
  static_assert(std::is_same<Sum<MyUnits, MyUnits>::type, MyUnits>::value);
}

TEST(BoostUnitMatrix, Difference) {  // NOLINT
  static_assert(std::is_same<Difference<TypeMatrix<2, 2, length>, TypeMatrix<2, 2, length>>::type, TypeMatrix<2, 2, length>>::value);
  using MyUnits = TypeMatrix<3, 2, length, area, volume, mass, force, energy>;
  static_assert(std::is_same<Difference<MyUnits, MyUnits>::type, MyUnits>::value);
}

TEST(BoostUnitMatrix, Product) {  // NOLINT
  static_assert(std::is_same<Product<TypeMatrix<2, 2, length>, TypeMatrix<2, 2, length>>::type, TypeMatrix<2, 2, area>>::value);
}

TEST(BoostUnitMatrix, Inverse) {  // NOLINT
  static_assert(std::is_same<Inverse<TypeMatrix<2, 2, boost::units::si::time>>::type, TypeMatrix<2, 2, frequency>>::value);
}
