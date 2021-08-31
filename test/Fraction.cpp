#include <gtest/gtest.h>

#include "ctm/core/Fraction.hpp"

#include "test_compilation_error.hpp"

using namespace ctm;  // NOLINT

TEST(TestFraction, Infinity){// NOLINT
                             ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(
                                 R"(
#include "ctm/core/Fraction.hpp"
int main() {
  using type = ctm::Fraction<3, 0>;
  return 0;
}
)",
                                 "The divisor must not be zero.")}

TEST(TestFraction, OneToInfinity) {  // NOLINT
  static_assert(std::is_same<Fraction<3, 1>, Fraction<3, 1>>::value);
  static_assert(std::is_same<Fraction<3, 2>, Fraction<3, 2>>::value);
  static_assert(std::is_same<Fraction<-6, -4>, Fraction<3, 2>>::value);
  static_assert(std::is_same<Fraction<9, 6>, Fraction<-3, -2>>::value);
}

TEST(TestFraction, One) {  // NOLINT
  static_assert(std::is_same<Fraction<1, 1>, Fraction<1, 1>>::value);
  static_assert(std::is_same<Fraction<-1, -1>, Fraction<1, 1>>::value);
  static_assert(std::is_same<Fraction<3, 3>, Fraction<1, 1>>::value);
}

TEST(TestFraction, ZeroToOne) {  // NOLINT
  static_assert(std::is_same<Fraction<2, 3>, Fraction<2, 3>>::value);
  static_assert(std::is_same<Fraction<-4, -6>, Fraction<2, 3>>::value);
  static_assert(std::is_same<Fraction<6, 9>, Fraction<-2, -3>>::value);
}

TEST(TestFraction, Zero) {  // NOLINT
  static_assert(std::is_same<Fraction<0, 1>, Fraction<0, 1>>::value);
  static_assert(std::is_same<Fraction<0, -3>, Fraction<0, 1>>::value);
  static_assert(std::is_same<Fraction<-0, -3>, Fraction<0, 1>>::value);
}

TEST(TestFraction, MinusOneToZero) {  // NOLINT
  static_assert(std::is_same<Fraction<-2, 3>, Fraction<-2, 3>>::value);
  static_assert(std::is_same<Fraction<-4, 6>, Fraction<-2, 3>>::value);
  static_assert(std::is_same<Fraction<6, -9>, Fraction<-2, 3>>::value);
}

TEST(TestFraction, MinusOne) {  // NOLINT
  static_assert(std::is_same<Fraction<1, -1>, Fraction<-1, 1>>::value);
  static_assert(std::is_same<Fraction<-3, 3>, Fraction<-1, 1>>::value);
}

TEST(TestFraction, MinusInfinityToMinusOne) {  // NOLINT
  static_assert(std::is_same<Fraction<-3, 1>, Fraction<-3, 1>>::value);
  static_assert(std::is_same<Fraction<3, -2>, Fraction<3, -2>>::value);
  static_assert(std::is_same<Fraction<6, -4>, Fraction<3, -2>>::value);
  static_assert(std::is_same<Fraction<-9, 6>, Fraction<-3, 2>>::value);
}

TEST(TestFraction, MinusInfinity) {  // NOLINT
  ASSERT_STATIC_ASSERTION_FAILURE_WITH_MESSAGE(
      R"(
#include "ctm/core/Fraction.hpp"
int main() {
  using type = ctm::Fraction<3, -0>;
  return 0;
}
)",
      "The divisor must not be zero.")
}
