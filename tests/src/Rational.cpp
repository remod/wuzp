#include <type_traits>

#include <gtest/gtest.h>

#include "wuzp/core/Rational.hpp"

using namespace wuzp;  // NOLINT

TEST(TestRational, AccessMembers) {  // NOLINT
  static_assert(Rational<3, 12>::Dividend == 1);
  static_assert(Rational<3, 12>::Divisor == 4);
}

TEST(TestRational, Value) {  // NOLINT
  EXPECT_EQ((Rational<3, 12>::value<double>()), 0.25);
  EXPECT_EQ((Rational<3, 12>::value<int>()), 0);
}

TEST(TestRational, OneToInfinity) {  // NOLINT
  static_assert(std::is_same<Rational<3, 1>, Rational<3, 1>>::value);
  static_assert(std::is_same<Rational<3, 2>, Rational<3, 2>>::value);
  static_assert(std::is_same<Rational<-6, -4>, Rational<3, 2>>::value);
  static_assert(std::is_same<Rational<9, 6>, Rational<-3, -2>>::value);
}

TEST(TestRational, One) {  // NOLINT
  static_assert(std::is_same<Rational<1, 1>, Rational<1, 1>>::value);
  static_assert(std::is_same<Rational<-1, -1>, Rational<1, 1>>::value);
  static_assert(std::is_same<Rational<3, 3>, Rational<1, 1>>::value);
}

TEST(TestRational, ZeroToOne) {  // NOLINT
  static_assert(std::is_same<Rational<2, 3>, Rational<2, 3>>::value);
  static_assert(std::is_same<Rational<-4, -6>, Rational<2, 3>>::value);
  static_assert(std::is_same<Rational<6, 9>, Rational<-2, -3>>::value);
}

TEST(TestRational, Zero) {  // NOLINT
  static_assert(std::is_same<Rational<0, 1>, Rational<0, 1>>::value);
  static_assert(std::is_same<Rational<0, -3>, Rational<0, 1>>::value);
  static_assert(std::is_same<Rational<-0, -3>, Rational<0, 1>>::value);
}

TEST(TestRational, MinusOneToZero) {  // NOLINT
  static_assert(std::is_same<Rational<-2, 3>, Rational<-2, 3>>::value);
  static_assert(std::is_same<Rational<-4, 6>, Rational<-2, 3>>::value);
  static_assert(std::is_same<Rational<6, -9>, Rational<-2, 3>>::value);
}

TEST(TestRational, MinusOne) {  // NOLINT
  static_assert(std::is_same<Rational<1, -1>, Rational<-1, 1>>::value);
  static_assert(std::is_same<Rational<-3, 3>, Rational<-1, 1>>::value);
}

TEST(TestRational, MinusInfinityToMinusOne) {  // NOLINT
  static_assert(std::is_same<Rational<-3, 1>, Rational<-3, 1>>::value);
  static_assert(std::is_same<Rational<3, -2>, Rational<3, -2>>::value);
  static_assert(std::is_same<Rational<6, -4>, Rational<3, -2>>::value);
  static_assert(std::is_same<Rational<-9, 6>, Rational<-3, 2>>::value);
}

TEST(TestRational, Identity) {  // NOLINT
  static_assert(std::is_same<typename Identity<Rational<-3, 2>>::type, Rational<1, 1>>::value);
}

TEST(TestRational, Sum) {  // NOLINT
  static_assert(std::is_same<typename Sum<Rational<-3, 12>, Rational<5, 12>>::type, Rational<1, 6>>::value);
}

TEST(TestRational, Difference) {  // NOLINT
  static_assert(std::is_same<typename Difference<Rational<-3, 12>, Rational<5, 12>>::type, Rational<-2, 3>>::value);
}

TEST(TestRational, negation) {  // NOLINT
  static_assert(std::is_same<typename Negation<Rational<-3, 12>>::type, Rational<3, 12>>::value);
}

TEST(TestRational, Product) {  // NOLINT
  static_assert(std::is_same<typename Product<Rational<-3, 12>, Rational<5, 12>>::type, Rational<-15, 144>>::value);
}

TEST(TestRational, Division) {  // NOLINT
  static_assert(std::is_same<typename Division<Rational<-3, 12>, Rational<5, 12>>::type, Rational<-3, 5>>::value);
}

TEST(TestRational, Inverse) {  // NOLINT
  static_assert(std::is_same<typename Inverse<Rational<-3, 2>>::type, Rational<-2, 3>>::value);
}

TEST(TestRational, LargeDivisor) {  // NOLINT
  // The reduction must not depend on the size of the numbers.
  static_assert(std::is_same<Rational<1, 1000>, Rational<1, 1000>>::value);
  static_assert(std::is_same<Rational<2000, 1000>, Rational<2, 1>>::value);
  static_assert(std::is_same<Rational<-1000000, 3000000>, Rational<-1, 3>>::value);
}

TEST(TestRational, NegativeDivisor) {  // NOLINT
  static_assert(std::is_same<Rational<4, -6>, Rational<-2, 3>>::value);
  static_assert(std::is_same<Rational<-4, -6>, Rational<2, 3>>::value);
}

TEST(TestRational, PowerOfZero) {  // NOLINT
  static_assert(std::is_same<Power<Rational<0, 1>, Rational<1, 2>>::type, Rational<0, 1>>::value);
  static_assert(std::is_same<Power<Rational<0, 1>, Rational<3, 1>>::type, Rational<0, 1>>::value);
}
