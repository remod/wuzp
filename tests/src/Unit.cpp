#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"

#include "test_compilation_error.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

using namespace wuzp;  // NOLINT

TEST(TestUnit, Sum) {  // NOLINT
  static_assert(std::is_same<Sum<Length, Length>::type, Length>::value);
  static_assert(std::is_same<Sum<AbsoluteLength, Length>::type, AbsoluteLength>::value);
  static_assert(std::is_same<Sum<Length, AbsoluteLength>::type, AbsoluteLength>::value);
}

TEST(TestUnit, SumInvalidTypes) {  // NOLINT
  const std::string code = R"(
#include "wuzp/core/traits.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

using namespace wuzp;

int main() {
  static_assert(std::is_same<Sum<AbsoluteLength, AbsoluteLength>::type, Length>::value);
  return 0;
}
)";
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(code, "Two absolute units cannot be summed.");
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(code, "no match for ‘operator+’");
#endif
}

TEST(TestUnit, Difference) {  // NOLINT
  static_assert(std::is_same<Difference<Length, Length>::type, Length>::value);
  static_assert(std::is_same<Difference<AbsoluteLength, Length>::type, AbsoluteLength>::value);
  static_assert(std::is_same<Difference<AbsoluteLength, AbsoluteLength>::type, Length>::value);
}

TEST(TestUnit, DifferenceInvalidTypes) {  // NOLINT
  const std::string code = R"(
#include "wuzp/core/traits.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

using namespace wuzp;

int main() {
  static_assert(std::is_same<Difference<Length, AbsoluteLength>::type, Length>::value);
  return 0;
}
)";
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(code, "An absolute unit cannot be subtracted from a relative unit.");
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(code, "no match for ‘operator-’");
#endif
}

TEST(TestUnit, Product) {  // NOLINT
  static_assert(std::is_same<Product<Length, Length>::type, Area>::value);
}

TEST(TestUnit, Division) {  // NOLINT
  static_assert(std::is_same<Division<Length, Time>::type, Velocity>::value);
}

TEST(TestUnit, Power) {  // NOLINT
  static_assert(std::is_same<Power<Length, 3>::type, Volume>::value);
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
