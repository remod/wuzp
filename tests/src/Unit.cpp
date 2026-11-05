#include <gtest/gtest.h>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/traits.hpp"

#include "Unit.hpp"

using namespace wuzp;  // NOLINT

TEST(TestUnit, Sum) {  // NOLINT
  static_assert(std::is_same<Sum<Length, Length>::type, Length>::value);
}

TEST(TestUnit, Difference) {  // NOLINT
  static_assert(std::is_same<Difference<Length, Length>::type, Length>::value);
}

TEST(TestUnit, Product) {  // NOLINT
  static_assert(std::is_same<Product<Length, Length>::type, Area>::value);
}

TEST(TestUnit, Division) {  // NOLINT
  static_assert(std::is_same<Division<Length, Time>::type, Velocity>::value);
}

TEST(TestUnit, Power) {  // NOLINT
  static_assert(std::is_same<Power<Length, Integer<3>>::type, Volume>::value);
}

TEST(TestUnit, Inverse) {  // NOLINT
  static_assert(std::is_same<Inverse<Time>::type, Frequency>::value);
}

TEST(TestUnit, Identity) {  // NOLINT
  static_assert(std::is_same<Identity<Length>::type, Dimensionless>::value);
  static_assert(std::is_same<Identity<Mass>::type, Dimensionless>::value);
  static_assert(std::is_same<Identity<Dimensionless>::type, Dimensionless>::value);
}

TEST(TestUnit, AreAnglesDimensionless) {  // NOLINT
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
  static_assert(std::is_same<Angle, Dimensionless>::value);
  static_assert(std::is_same<Product<Angle, Length>::type, Length>::value);
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
  static_assert(!std::is_same<Angle, Dimensionless>::value);
  static_assert(!std::is_same<Product<Angle, Length>::type, Length>::value);
#endif
}
