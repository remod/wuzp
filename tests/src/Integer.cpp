#include <gtest/gtest.h>

#include "wuzp/core/Integer.hpp"

using namespace wuzp;  // NOLINT

TEST(TestInteger, CompareToRational) {  // NOLINT
  static_assert(std::is_same<Integer<3>, Rational<3, 1>>::value);
  static_assert(std::is_same<Integer<0>, Rational<0, 2>>::value);
  static_assert(std::is_same<Integer<-3>, Rational<6, -2>>::value);
}

TEST(TestInteger, Identity) {  // NOLINT
  static_assert(std::is_same<typename Identity<Integer<-3>>::type, Integer<1>>::value);
}

TEST(TestInteger, Sum) {  // NOLINT
  static_assert(std::is_same<typename Sum<Integer<-3>, Integer<5>>::type, Integer<2>>::value);
}

TEST(TestInteger, negation) {  // NOLINT
  static_assert(std::is_same<typename Negation<Integer<-3>>::type, Integer<3>>::value);
}

TEST(TestInteger, Difference) {  // NOLINT
  static_assert(std::is_same<typename Difference<Integer<-3>, Integer<5>>::type, Integer<-8>>::value);
}

TEST(TestInteger, Product) {  // NOLINT
  static_assert(std::is_same<typename Product<Integer<-3>, Integer<5>>::type, Integer<-15>>::value);
}

TEST(TestInteger, Division) {  // NOLINT
  static_assert(std::is_same<typename Division<Integer<-3>, Integer<5>>::type, Rational<-3, 5>>::value);
}

TEST(TestInteger, Inverse) {  // NOLINT
  static_assert(std::is_same<typename Inverse<Integer<-3>>::type, Rational<-1, 3>>::value);
}
