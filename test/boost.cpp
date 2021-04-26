#include <gtest/gtest.h>

#include "ctm/TypeMatrix.hpp"
#include "ctm/boost.hpp"

using boost::units::si::area;
using boost::units::si::energy;
using boost::units::si::force;
using boost::units::si::frequency;
using boost::units::si::length;
using boost::units::si::mass;
using boost::units::si::volume;
using ctm::Inverse;
using ctm::Product;
using ctm::Sum;
using ctm::TypeMatrix;

TEST(Boost_Unit, Sum) {  // NOLINT
  static_assert(std::is_same<Sum<length>::type, length>::value);
  static_assert(std::is_same<Sum<length, length>::type, length>::value);
  static_assert(std::is_same<Sum<length, length, length>::type, length>::value);
}

TEST(Boost_Unit, Product) {  // NOLINT
  static_assert(std::is_same<Product<length, length>::type, area>::value);
}

TEST(Boost_Unit, Inverse) {  // NOLINT
  static_assert(std::is_same<Inverse<boost::units::si::time>::type, frequency>::value);
}

TEST(Boost_UnitMatrix, Sum) {  // NOLINT
  static_assert(std::is_same<Sum<TypeMatrix<2, 2, length>, TypeMatrix<2, 2, length>>::type, TypeMatrix<2, 2, length>>::value);
  using MyUnits = TypeMatrix<3, 2, length, area, volume, mass, force, energy>;
  static_assert(std::is_same<Sum<MyUnits, MyUnits>::type, MyUnits>::value);
}

TEST(Boost_UnitMatrix, Product) {  // NOLINT
  static_assert(std::is_same<Product<TypeMatrix<2, 2, length>, TypeMatrix<2, 2, length>>::type, TypeMatrix<2, 2, area>>::value);
}
