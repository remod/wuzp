#include <gtest/gtest.h>

#include "wuzp/core/helpers.hpp"

using wuzp::areSame;

TEST(TestHelpers, AreSame) {  // NOLINT
  EXPECT_TRUE((wuzp::areSame<double>));
  EXPECT_TRUE((wuzp::areSame<double, double>));
  EXPECT_TRUE((wuzp::areSame<double, double, double>));
  EXPECT_FALSE((wuzp::areSame<double, double, int>));
}
