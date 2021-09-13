#include <gtest/gtest.h>

#include "wuzp/core/helpers.hpp"

using namespace wuzp;  // NOLINT

TEST(TestHelpers, AreSame) {  // NOLINT
  EXPECT_TRUE((areSame<double>));
  EXPECT_TRUE((areSame<double, double>));
  EXPECT_TRUE((areSame<double, double, double>));
  EXPECT_FALSE((areSame<double, double, int>));
}
