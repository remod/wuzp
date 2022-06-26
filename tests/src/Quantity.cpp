#include <gtest/gtest.h>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/traits.hpp"

#include "test_compilation_error.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
using namespace wuzp_eigen;  // NOLINT

using namespace wuzp;  // NOLINT

using TimeQuantity = QuantityD<Time>;
using DimensionlessQuantity = QuantityD<Dimensionless>;
using FrequencyQuantity = QuantityD<Frequency>;
using AngleQuantity = QuantityD<Angle>;
using LengthQuantity = QuantityD<Length>;

TEST(TestQuantity, CopyConstructor) {  // NOLINT
  const TimeQuantity time(1.0);
  EXPECT_EQ(time.getValue(), 1.0);
  const TimeQuantity time2(time);  // NOLINT
  EXPECT_EQ(time2.getValue(), 1.0);
  EXPECT_EQ(time2, time);
}

TEST(TestQuantity, ValueAccess) {  // NOLINT
  TimeQuantity time(1.0);
  EXPECT_EQ(time.getValue(), 1.0);
  time.setValue(2.0);
  EXPECT_EQ(time.getValue(), 2.0);
}

TEST(TestQuantity, Sum) {  // NOLINT
  EXPECT_EQ(TimeQuantity(1.0) + TimeQuantity(2.0), TimeQuantity(3.0));
}

TEST(TestQuantity, SumMixedQuantities) {  // NOLINT
  const std::string code = R"(
#include "wuzp/core/Quantities.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#include "wuzp_eigen/ValueMatrix.hpp"
using namespace wuzp_eigen;  // NOLINT

using namespace wuzp;  // NOLINT

using TimeQuantity = QuantityD<Time>;
using LengthQuantity = QuantityD<Length>;

int main() {
  [[maybe_unused]] const auto invalid = TimeQuantity(2.0) + LengthQuantity(3.0);
  return 0;
}
)";
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(code, "Units do not match.")
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
  ASSERT_COMPILATION_ERROR(code)
#endif
}

TEST(TestQuantity, Difference) {  // NOLINT
  EXPECT_EQ(TimeQuantity(3.0) - TimeQuantity(2.0), TimeQuantity(1.0));
}

TEST(TestQuantity, DifferenceMixedQuantities) {  // NOLINT
  const std::string code = R"(
#include "wuzp/core/Quantities.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#include "wuzp_eigen/ValueMatrix.hpp"
using namespace wuzp_eigen;  // NOLINT

using namespace wuzp;  // NOLINT

using TimeQuantity = QuantityD<Time>;
using LengthQuantity = QuantityD<Length>;

int main() {
  [[maybe_unused]] const auto invalid = TimeQuantity(2.0) - LengthQuantity(3.0);
  return 0;
}
)";
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(code, "Units do not match.")
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
  ASSERT_COMPILATION_ERROR(code)
#endif
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

TEST(TestQuantity, Chain) {  // NOLINT
  EXPECT_EQ(TimeQuantity(1.0) + TimeQuantity(2.0) * DimensionlessQuantity(3.0), TimeQuantity(7.0));
}

TEST(TestQuantity, Inverse) {  // NOLINT
  EXPECT_EQ(TimeQuantity(2.0).inverse(), FrequencyQuantity(0.5));
}

TEST(TestQuantity, AreAnglesDimensionless) {  // NOLINT
  const LengthQuantity radius(2.0);
  const AngleQuantity angle(3.0);
  [[maybe_unused]] const LengthQuantity arcLength = angle *
                                                    radius
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
                                                    // With boost, angles are not dimensionless. We need to "normalize" the result.
                                                    / AngleQuantity(1.0)
#endif
      ;
}

#endif
