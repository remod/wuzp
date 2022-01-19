#include <gtest/gtest.h>

#include "wuzp/core/Coordinate.hpp"
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

using namespace wuzp;  // NOLINT

using AbsoluteTimeQuantity = Quantity<double, AbsoluteTime>;
using TimeQuantity = Quantity<double, Time>;
using FrequencyQuantity = Quantity<double, Frequency>;
using DimensionlessQuantity = Quantity<double, Dimensionless>;
struct EpochName {};
using Epoch = CoordinateSystem<EpochName>;
using TimeSinceEpoch = Coordinate<AbsoluteTimeQuantity, Epoch>;
using DurationInEpoch = Coordinate<TimeQuantity, Epoch>;
using FrequencyInEpoch = Coordinate<FrequencyQuantity, Epoch>;
using DimensionlessInEpoch = Coordinate<DimensionlessQuantity, Epoch>;

// TODO(remod): Add use case with different ratios (s vs ms) --> duration != duration.

TEST(TestCoordinate, CopyConstructor) {  // NOLINT
  const auto time = TimeSinceEpoch(1.0);
  EXPECT_EQ(time.getValue(), 1.0);
  const auto time2(time);
  EXPECT_EQ(time2.getValue(), 1.0);
  EXPECT_EQ(time2, time);
}

TEST(TestCoordinate, QuantityAccess) {  // NOLINT
  auto time = TimeSinceEpoch(1.0);
  EXPECT_EQ(time.getQuantity(), AbsoluteTimeQuantity(1.0));
  time.setQuantity(AbsoluteTimeQuantity(2.0));
  EXPECT_EQ(time.getQuantity(), AbsoluteTimeQuantity(2.0));
}

TEST(TestCoordinate, ValueAccess) {  // NOLINT
  auto time = TimeSinceEpoch(1.0);
  EXPECT_EQ(time.getValue(), 1.0);
  time.setValue(2.0);
  EXPECT_EQ(time.getValue(), 2.0);
}

TEST(TestCoordinate, Addition) {  // NOLINT
  EXPECT_EQ(TimeSinceEpoch(1.0) + DurationInEpoch(2.0), TimeSinceEpoch(3.0));
}

TEST(TestCoordinate, Subtraction) {  // NOLINT
  EXPECT_EQ(TimeSinceEpoch(3.0) - DurationInEpoch(2.0), TimeSinceEpoch(1.0));
}

TEST(TestCoordinate, EqualityOperator) {  // NOLINT
  EXPECT_EQ(TimeSinceEpoch(1.0), TimeSinceEpoch(1.0));
}

TEST(TestCoordinate, InEqualityOperator) {  // NOLINT
  EXPECT_NE(TimeSinceEpoch(1.0), TimeSinceEpoch(2.0));
}

TEST(TestCoordinate, Product) {  // NOLINT
  EXPECT_EQ(DurationInEpoch(2.0) * FrequencyInEpoch(3.0), DimensionlessInEpoch(6.0));
}

TEST(TestCoordinate, Chain) {  // NOLINT
  EXPECT_EQ(TimeSinceEpoch(1.0) + DurationInEpoch(2.0) * DimensionlessInEpoch(3.0), TimeSinceEpoch(7.0));
}

TEST(TestCoordinate, Inverse) {  // NOLINT
  EXPECT_EQ(DurationInEpoch(2.0).inverse(), FrequencyInEpoch(0.5));
}
