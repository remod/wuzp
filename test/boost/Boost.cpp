#include <gtest/gtest.h>

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_CTM
#include "ctm/unit_impl/ctm.hpp"
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include <boost/units/absolute.hpp>
#include "ctm/boost/boost.hpp"
#endif

#include "ctm/core/TypeMatrix.hpp"
#include "ctm/core/UnitMatrix.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_CTM
using namespace ctm::units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
using Length = boost::units::si::length;
using Time = boost::units::si::time;
using Area = boost::units::si::area;
using Mass = boost::units::si::mass;
using Force = boost::units::si::force;
using Energy = boost::units::si::energy;
using Dimensionless = boost::units::si::dimensionless;
using Velocity = boost::units::si::velocity;
using Volume = boost::units::si::volume;
using Frequency = boost::units::si::frequency;
template <typename Unit>
using Absolute = boost::units::absolute<Unit>;
#endif
using ctm::Difference;
using ctm::Inverse;
using ctm::Product;
using ctm::Sum;
using ctm::UnitMatrix;
using ctm::UnitTraits;

TEST(BoostUnit, Sum) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Sum<Length, Length>::type, Length>::value);
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST  // TODO(remod): Also support absolute units in CTM
  static_assert(std::is_same<UnitTraits::Sum<Absolute<Length>, Length>::type, Absolute<Length>>::value);
  static_assert(std::is_same<UnitTraits::Sum<Length, Absolute<Length>>::type, Absolute<Length>>::value);
#endif
}

TEST(BoostUnit, Difference) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Difference<Length, Length>::type, Length>::value);
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST  // TODO(remod): Also support absolute units in CTM
  static_assert(std::is_same<UnitTraits::Difference<Absolute<Length>, Length>::type, Absolute<Length>>::value);
#endif
}

TEST(BoostUnit, Product) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Product<Length, Length>::type, Area>::value);
}

TEST(BoostUnitMatrix, Division) {  // NOLINT
  //  static_assert(std::is_same<UnitTraits::Division<Length, Time>::type, Absolute<units::velocity>>::value);
  static_assert(std::is_same<UnitTraits::Division<Length, Time>::type, Velocity>::value);
}

TEST(BoostUnit, Inverse) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Inverse<Time>::type, Frequency>::value);
}

TEST(BoostUnit, Identity) {  // NOLINT
  static_assert(std::is_same<UnitTraits::Identity<Length>::type, Dimensionless>::value);
  static_assert(std::is_same<UnitTraits::Identity<Mass>::type, Dimensionless>::value);
  static_assert(std::is_same<UnitTraits::Identity<Dimensionless>::type, Dimensionless>::value);
}

TEST(BoostUnitMatrix, Sum) {  // NOLINT
  static_assert(std::is_same<Sum<UnitMatrix<2, 2, Length>, UnitMatrix<2, 2, Length>>::type, UnitMatrix<2, 2, Length>>::value);
  using MyUnits = UnitMatrix<3, 2, Length, Area, Volume, Mass, Force, Energy>;
  static_assert(std::is_same<Sum<MyUnits, MyUnits>::type, MyUnits>::value);
}

TEST(BoostUnitMatrix, Difference) {  // NOLINT
  static_assert(std::is_same<Difference<UnitMatrix<2, 2, Length>, UnitMatrix<2, 2, Length>>::type, UnitMatrix<2, 2, Length>>::value);
  using MyUnits = UnitMatrix<3, 2, Length, Area, Volume, Mass, Force, Energy>;
  static_assert(std::is_same<Difference<MyUnits, MyUnits>::type, MyUnits>::value);
}

TEST(BoostUnitMatrix, Product) {  // NOLINT
  static_assert(std::is_same<Product<UnitMatrix<2, 2, Length>, UnitMatrix<2, 2, Length>>::type, UnitMatrix<2, 2, Area>>::value);
}

TEST(BoostUnitMatrix, Inverse) {  // NOLINT
  static_assert(std::is_same<Inverse<UnitMatrix<2, 2, Time>>::type, UnitMatrix<2, 2, Frequency>>::value);
}

// TEST(BoostUnitMatrix, Test) {  // NOLINT
//  using boost::units::quantity;
//  quantity<units::plane_angle> x;
//  x = 3.*units::radian;
//  x = 3.*units::si_dimensionless;
//  x = 3.*units::radian;
////  quantity<units::plane_angle> y = 3.*units::kilogram;
//}
