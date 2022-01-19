#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Quantity.hpp"
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Quantity.hpp"
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#include "test_compilation_error.hpp"

using namespace wuzp;  // NOLINT

using AbsoluteTimeQuantity = Quantity<double, AbsoluteTime>;
using TimeQuantity = Quantity<double, Time>;
using DimensionlessQuantity = Quantity<double, Dimensionless>;
using FrequencyQuantity = Quantity<double, Frequency>;
using AngleQuantity = Quantity<double, Angle>;
using LengthQuantity = Quantity<double, Length>;

TEST(TestQuantity, CopyConstructor) {  // NOLINT
  const auto time = TimeQuantity(1.0);
  EXPECT_EQ(time.getValue(), 1.0);
  const auto time2(time);
  EXPECT_EQ(time2.getValue(), 1.0);
  EXPECT_EQ(time2, time);
}

TEST(TestQuantity, ValueAccess) {  // NOLINT
  auto time = TimeQuantity(1.0);
  EXPECT_EQ(time.getValue(), 1.0);
  time.setValue(2.0);
  EXPECT_EQ(time.getValue(), 2.0);
}

TEST(TestQuantity, Addition) {  // NOLINT
  EXPECT_EQ(TimeQuantity(1.0) + TimeQuantity(2.0), TimeQuantity(3.0));
}

TEST(TestQuantity, Subtraction) {  // NOLINT
  EXPECT_EQ(TimeQuantity(3.0) - TimeQuantity(2.0), TimeQuantity(1.0));
}

TEST(TestQuantity, EqualityOperator) {  // NOLINT
  EXPECT_EQ(TimeQuantity(1.0), TimeQuantity(1.0));
}

TEST(TestQuantity, InEqualityOperator) {  // NOLINT
  EXPECT_NE(TimeQuantity(1.0), TimeQuantity(2.0));
}

TEST(TestQuantity, Product) {  // NOLINT
  EXPECT_EQ(TimeQuantity(2.0) * FrequencyQuantity(3.0), DimensionlessQuantity(6.0));
}

TEST(TestQuantity, AbsoluteProduct) {  // NOLINT
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(R"(
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Quantity.hpp"
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Quantity.hpp"
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

using namespace wuzp;  // NOLINT

using AbsoluteTimeQuantity = Quantity<double, AbsoluteTime>;
using FrequencyQuantity = Quantity<double, Frequency>;

int main() {
  [[maybe_unused]] const auto invalid = AbsoluteTimeQuantity(2.0) * FrequencyQuantity(3.0);
  return 0;
}
)",
                                               "An absolute unit cannot appear in a multiplication.")
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
  using AbsoluteTimeQuantity = Quantity<double, AbsoluteTime>;
  using FrequencyQuantity = Quantity<double, Frequency>;
  [[maybe_unused]] const auto valid = AbsoluteTimeQuantity(2.0) * FrequencyQuantity(3.0);
#endif
}

TEST(TestQuantity, Chain) {  // NOLINT
  EXPECT_EQ(TimeQuantity(1.0) + TimeQuantity(2.0) * DimensionlessQuantity(3.0), TimeQuantity(7.0));
}

TEST(TestQuantity, Inverse) {  // NOLINT
  EXPECT_EQ(TimeQuantity(2.0).inverse(), FrequencyQuantity(0.5));
}

TEST(TestQuantity, AbsoluteInverse) {  // NOLINT
  const std::string code = R"(
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Quantity.hpp"
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Quantity.hpp"
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

using namespace wuzp;  // NOLINT

using AbsoluteTimeQuantity = Quantity<double, AbsoluteTime>;

int main() {
  [[maybe_unused]] const auto invalid = AbsoluteTimeQuantity(2.0).inverse();
  return 0;
}
)";
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(code, "An absolute unit cannot be powered.")
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
  ASSERT_COMPILATION_ERROR_WITH_MESSAGE(code, "implicit instantiation of undefined template")
#endif
}

TEST(TestQuantity, AreAnglesDimensionless) {  // NOLINT
  const auto radius = LengthQuantity(2.0);
  const auto angle = AngleQuantity(3.0);
  [[maybe_unused]] const LengthQuantity arcLength = angle *
                                                    radius
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                                                    // With boost, angles are not dimensionless. We need to "normalize" the result.
                                                    / AngleQuantity(1.0)
#endif
      ;
}
