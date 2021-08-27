#include <gtest/gtest.h>

#include "ctm/core/helpers.hpp"

using ctm::areSame;

TEST(TestHelpers, AreSame) {  // NOLINT
  EXPECT_TRUE((ctm::areSame<double>));
  EXPECT_TRUE((ctm::areSame<double, double>));
  EXPECT_TRUE((ctm::areSame<double, double, double>));
  EXPECT_FALSE((ctm::areSame<double, double, int>));
}
