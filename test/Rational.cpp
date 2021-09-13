#include <gtest/gtest.h>

#include "wuzp/core/Rational.hpp"

#include "test_compilation_error.hpp"

using namespace wuzp;  // NOLINT

TEST(TestRational, Infinity){// NOLINT
                             ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(
                                 R"(
#include "wuzp/core/Rational.hpp"
int main() {
  using type = wuzp::Rational<3, 0>;
  return 0;
}
)",
                                 "The divisor must not be zero.")}

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

TEST(TestRational, MinusInfinity){// NOLINT
                                  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(
                                      R"(
#include "wuzp/core/Rational.hpp"
int main() {
  using type = wuzp::Rational<3, -0>;
  return 0;
}
)",
                                      "The divisor must not be zero.")}

TEST(TestRational, Identity) {  // NOLINT
  static_assert(std::is_same<typename RationalTraits::Identity<Rational<-3, 2>>::type, Rational<1, 1>>::value);
}

TEST(TestRational, Sum) {  // NOLINT
  static_assert(std::is_same<typename RationalTraits::Sum<Rational<-3, 12>, Rational<5, 12>>::type, Rational<1, 6>>::value);
}

TEST(TestRational, Difference) {  // NOLINT
  static_assert(std::is_same<typename RationalTraits::Difference<Rational<-3, 12>, Rational<5, 12>>::type, Rational<-2, 3>>::value);
}

TEST(TestRational, Product) {  // NOLINT
  static_assert(std::is_same<typename RationalTraits::Product<Rational<-3, 12>, Rational<5, 12>>::type, Rational<-15, 144>>::value);
}

TEST(TestRational, Division) {  // NOLINT
  static_assert(std::is_same<typename RationalTraits::Division<Rational<-3, 12>, Rational<5, 12>>::type, Rational<-3, 5>>::value);
}

TEST(TestRational, Inverse) {  // NOLINT
  static_assert(std::is_same<typename RationalTraits::Inverse<Rational<-3, 2>>::type, Rational<-2, 3>>::value);
}
