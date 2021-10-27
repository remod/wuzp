#include <gtest/gtest.h>

#include "wuzp/core/helpers.hpp"

using namespace wuzp;  // NOLINT

template <typename T>
struct MyTemplate {};

TEST(TestHelpers, IsSpecialization) {  // NOLINT
  static_assert(is_specialization<MyTemplate<int>, MyTemplate>::value);
  static_assert(!is_specialization<double, MyTemplate>::value);
}

TEST(TestHelpers, AreSame) {  // NOLINT
  static_assert(areSame<double>);
  static_assert(areSame<double, double>);
  static_assert(areSame<double, double, double>);
  static_assert(!areSame<double, double, int>);
}
