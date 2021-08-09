#include <gtest/gtest.h>

#include "ctm/core/Quantity.hpp"
#include "ctm/core/traits.hpp"
#include "ctm/units/Unit.hpp"

using ctm::Quantity;
using Time = Quantity<double, ctm::units::Time>;
using Frequency = Quantity<double, ctm::units::Frequency>;
using Dimensionless = Quantity<double, ctm::units::Dimensionless>;

TEST(TestQuantity, Addition) {  // NOLINT
  EXPECT_EQ(Time(1.0) + Time(2.0), Time(3.0));
}

TEST(TestQuantity, Subtraction) {  // NOLINT
  EXPECT_EQ(Time(3.0) - Time(2.0), Time(1.0));
}

TEST(TestQuantity, EqualityOperator) {  // NOLINT
  EXPECT_EQ(Time(1.0), Time(1.0));
}

TEST(TestQuantity, InEqualityOperator) {  // NOLINT
  EXPECT_NE(Time(1.0), Time(2.0));
}

TEST(TestQuantity, Product) {  // NOLINT
  EXPECT_EQ(Time(2.0) * Frequency(3.0), Dimensionless(6.0));
}

TEST(TestQuantity, Chain) {  // NOLINT
  EXPECT_EQ(Time(1.0) + Time(2.0) * Time(3.0), Time(7.0));
}

TEST(TestQuantity, Inverse) {  // NOLINT
  EXPECT_EQ(Time(2.0).inverse(), Frequency(0.5));
}
