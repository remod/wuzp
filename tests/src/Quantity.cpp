#include <gtest/gtest.h>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/traits.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

using TimeQuantity = QuantityD<Time>;
using DimensionlessQuantity = QuantityD<Dimensionless>;
using FrequencyQuantity = QuantityD<Frequency>;
using AngleQuantity = QuantityD<Angle>;
using LengthQuantity = QuantityD<Length>;

TEST(TestQuantity, CopyConstructor) {  // NOLINT
  const TimeQuantity time{1.0};
  EXPECT_EQ(time.getValue(), 1.0);
  const TimeQuantity time2{time};  // NOLINT
  EXPECT_EQ(time2.getValue(), 1.0);
  EXPECT_EQ(time2, time);
}

TEST(TestQuantity, ValueAccess) {  // NOLINT
  TimeQuantity time{1.0};
  EXPECT_EQ(time.getValue(), 1.0);
  time.setValue(2.0);
  EXPECT_EQ(time.getValue(), 2.0);
}

TEST(TestQuantity, Sum) {  // NOLINT
  EXPECT_EQ(TimeQuantity{1.0} + TimeQuantity{2.0}, TimeQuantity(3.0));
}

TEST(TestQuantity, Difference) {  // NOLINT
  EXPECT_EQ(TimeQuantity{3.0} - TimeQuantity{2.0}, TimeQuantity{1.0});
}

TEST(TestQuantity, EqualityOperator) {  // NOLINT
  EXPECT_EQ(TimeQuantity{1.0}, TimeQuantity{1.0});
}

TEST(TestQuantity, InequalityOperator) {  // NOLINT
  EXPECT_NE(TimeQuantity{1.0}, TimeQuantity{2.0});
}

TEST(TestQuantity, Product) {  // NOLINT
  DimensionlessQuantity x{TimeQuantity{2.0} * FrequencyQuantity(3.0)};
  EXPECT_EQ(TimeQuantity{2.0} * FrequencyQuantity{3.0}, DimensionlessQuantity(6.0));
}

TEST(TestQuantity, Chain) {  // NOLINT
  EXPECT_EQ(TimeQuantity{1.0} + TimeQuantity{2.0} * DimensionlessQuantity{3.0}, TimeQuantity{7.0});
}

#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestQuantity, Inverse) {  // NOLINT
  EXPECT_EQ(TimeQuantity{2.0}.inverse(), FrequencyQuantity{0.5});
}
#endif

TEST(TestQuantity, AreAnglesDimensionless) {  // NOLINT
  const LengthQuantity radius{2.0};
  const AngleQuantity angle{3.0};
  [[maybe_unused]] const LengthQuantity arcLength{angle * radius};
}
