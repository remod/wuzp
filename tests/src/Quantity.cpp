#include <cmath>
#include <concepts>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <type_traits>

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
using AreaQuantityI = QuantityI<Area>;
using LengthQuantityI = QuantityI<Length>;
using LengthQuantityLL = Quantity<ValueMatrix<long long, 1, 1>, Length>;
using AreaQuantityLL = Quantity<ValueMatrix<long long, 1, 1>, Area>;
using VolumeQuantityLL = Quantity<ValueMatrix<long long, 1, 1>, Volume>;
using LengthQuantityULL = Quantity<ValueMatrix<unsigned long long, 1, 1>, Length>;

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
  // An int is converted into the double values; a double for int values is rejected (compile test Quantity.SetWiderValue).
  time.setValue(3);
  EXPECT_EQ(time.getValue(), 3.0);
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
  const DimensionlessQuantity x{TimeQuantity{2.0} * FrequencyQuantity(3.0)};
  EXPECT_EQ(x, DimensionlessQuantity(6.0));
  EXPECT_EQ(TimeQuantity{2.0} * FrequencyQuantity{3.0}, DimensionlessQuantity(6.0));
}

// An arithmetic scalar of another type is converted into the value type of the quantity, if their common type is the value type.
TEST(TestQuantity, ProductWithScalarOfAnotherType) {  // NOLINT
  EXPECT_EQ(TimeQuantity{1.5} * 2, TimeQuantity{3.0});
  EXPECT_EQ(2 * TimeQuantity{1.5}, TimeQuantity{3.0});
  EXPECT_EQ(TimeQuantity{3.0} / 2, TimeQuantity{1.5});
  static_assert(std::is_same_v<decltype(TimeQuantity{1.5} * 2), TimeQuantity>);
  static_assert(std::is_same_v<decltype(TimeQuantity{3.0} / 2), TimeQuantity>);
}

TEST(TestQuantity, Chain) {  // NOLINT
  EXPECT_EQ(TimeQuantity{1.0} + TimeQuantity{2.0} * DimensionlessQuantity{3.0}, TimeQuantity{7.0});
}

TEST(TestQuantity, Inverse) {  // NOLINT
  EXPECT_EQ(TimeQuantity{2.0}.inverse(), FrequencyQuantity{0.5});
}

TEST(TestQuantity, AreAnglesDimensionless) {  // NOLINT
  const LengthQuantity radius{2.0};
  const AngleQuantity angle{3.0};
  [[maybe_unused]] const LengthQuantity arcLength{angle * radius};
}

// Operations on the result of an operation, here a sum (#64).
TEST(TestQuantity, OperationsOnSums) {  // NOLINT
  const AngleQuantity lhs{0.1};
  const AngleQuantity rhs{0.2};
  EXPECT_TRUE((-(lhs + rhs)).isApprox(AngleQuantity{-0.3}, 1e-12));
  EXPECT_TRUE(sin(lhs + rhs).isApprox(DimensionlessQuantity{std::sin(0.3)}, 1e-12));
  EXPECT_TRUE(cos(lhs + rhs).isApprox(DimensionlessQuantity{std::cos(0.3)}, 1e-12));
}

// The sine, the cosine and the tangent are computed in the value type, also for float. The compile tests Quantity.*OfIntegerAngle
// check that integer values are rejected, and Quantity.*OfLength that units other than dimensionless ones are.
TEST(TestQuantity, SineCosineAndTangentOfFloats) {  // NOLINT
  const QuantityF<Angle> angle{0.5F};
  const QuantityF<Dimensionless> sine = sin(angle);
  const QuantityF<Dimensionless> cosine = cos(angle);
  const QuantityF<Dimensionless> tangent = tan(angle);
  EXPECT_TRUE(sine.isApprox(QuantityF<Dimensionless>{std::sin(0.5F)}, 1e-6F));
  EXPECT_TRUE(cosine.isApprox(QuantityF<Dimensionless>{std::cos(0.5F)}, 1e-6F));
  EXPECT_TRUE(tangent.isApprox(QuantityF<Dimensionless>{std::tan(0.5F)}, 1e-6F));
}

// The trigonometric functions take any dimensionless quantity, e.g. also a ratio of lengths, and the inverse functions return
// angles, which are dimensionless (see "Angles are dimensionless" in DESIGN.md).
TEST(TestQuantity, TrigonometricFunctions) {  // NOLINT
  const AngleQuantity angle{0.5};
  const DimensionlessQuantity tangent = tan(angle);
  EXPECT_TRUE(tangent.isApprox(DimensionlessQuantity{std::tan(0.5)}, 1e-12));
  const DimensionlessQuantity ratio = LengthQuantity{1.0} / LengthQuantity{2.0};
  EXPECT_TRUE(sin(ratio).isApprox(DimensionlessQuantity{std::sin(0.5)}, 1e-12));
  const AngleQuantity arcSine = asin(ratio);
  const AngleQuantity arcCosine = acos(ratio);
  const AngleQuantity arcTangent = atan(ratio);
  EXPECT_TRUE(arcSine.isApprox(AngleQuantity{std::asin(0.5)}, 1e-12));
  EXPECT_TRUE(arcCosine.isApprox(AngleQuantity{std::acos(0.5)}, 1e-12));
  EXPECT_TRUE(arcTangent.isApprox(AngleQuantity{std::atan(0.5)}, 1e-12));
  // The inverse functions undo the functions in their ranges, [-pi/2, pi/2] for asin and atan and [0, pi] for acos.
  EXPECT_TRUE(asin(sin(angle)).isApprox(angle, 1e-12));
  EXPECT_TRUE(acos(cos(angle)).isApprox(angle, 1e-12));
  EXPECT_TRUE(atan(tan(angle)).isApprox(angle, 1e-12));
  EXPECT_TRUE(acos(DimensionlessQuantity{-1.0}).isApprox(AngleQuantity{std::numbers::pi}, 1e-12));
  // Outside [-1, 1], the arc sine and the arc cosine are NaN, like std::asin and std::acos.
  EXPECT_TRUE(std::isnan(asin(DimensionlessQuantity{2.0}).getValue()));
  EXPECT_TRUE(std::isnan(acos(DimensionlessQuantity{2.0}).getValue()));
  EXPECT_TRUE((asin(QuantityF<Dimensionless>{0.5F}).isApprox(QuantityF<Angle>{std::asin(0.5F)}, 1e-6F)));
}

// atan2(y, x) is the angle of the point (x, y) from the positive x axis, in [-pi, pi], of two quantities with the same unit, like
// std::atan2: on its branch cut, the negative x axis, the sign of a zero y decides between pi and -pi.
TEST(TestQuantity, Atan2) {  // NOLINT
  using std::numbers::pi;
  const AngleQuantity angle = atan2(LengthQuantity{1.0}, LengthQuantity{1.0});
  EXPECT_TRUE(angle.isApprox(AngleQuantity{pi / 4.0}, 1e-12));
  EXPECT_TRUE(atan2(LengthQuantity{1.0}, LengthQuantity{-1.0}).isApprox(AngleQuantity{3.0 * pi / 4.0}, 1e-12));
  EXPECT_TRUE(atan2(LengthQuantity{-1.0}, LengthQuantity{-1.0}).isApprox(AngleQuantity{-3.0 * pi / 4.0}, 1e-12));
  EXPECT_TRUE(atan2(LengthQuantity{-1.0}, LengthQuantity{1.0}).isApprox(AngleQuantity{-pi / 4.0}, 1e-12));
  EXPECT_EQ(atan2(LengthQuantity{0.0}, LengthQuantity{1.0}), AngleQuantity{0.0});
  EXPECT_EQ(atan2(LengthQuantity{0.0}, LengthQuantity{-1.0}), AngleQuantity{pi});
  EXPECT_EQ(atan2(LengthQuantity{-0.0}, LengthQuantity{-1.0}), AngleQuantity{-pi});
  // Any unit, e.g. of the components of a velocity, and float values.
  EXPECT_TRUE(atan2(QuantityD<Velocity>{1.0}, QuantityD<Velocity>{0.0}).isApprox(AngleQuantity{pi / 2.0}, 1e-12));
  EXPECT_TRUE(atan2(QuantityF<Length>{1.0F}, QuantityF<Length>{1.0F}).isApprox(QuantityF<Angle>{std::atan2(1.0F, 1.0F)}, 1e-6F));
}

// The exponential function and the natural logarithm of dimensionless quantities, like std::exp and std::log.
TEST(TestQuantity, ExponentialAndLogarithm) {  // NOLINT
  const DimensionlessQuantity exponential = exp(DimensionlessQuantity{1.0});
  EXPECT_TRUE(exponential.isApprox(DimensionlessQuantity{std::numbers::e}, 1e-12));
  EXPECT_EQ(exp(DimensionlessQuantity{0.0}), DimensionlessQuantity{1.0});
  // E.g. the ratio of two times, as in exponential decay.
  const DimensionlessQuantity logarithm = log(TimeQuantity{2.0} / TimeQuantity{1.0});
  EXPECT_TRUE(logarithm.isApprox(DimensionlessQuantity{std::numbers::ln2}, 1e-12));
  EXPECT_TRUE(log(exp(DimensionlessQuantity{0.3})).isApprox(DimensionlessQuantity{0.3}, 1e-12));
  EXPECT_EQ(log(DimensionlessQuantity{1.0}), DimensionlessQuantity{0.0});
  EXPECT_EQ(log(DimensionlessQuantity{0.0}), DimensionlessQuantity{-std::numeric_limits<double>::infinity()});
  EXPECT_TRUE(std::isnan(log(DimensionlessQuantity{-1.0}).getValue()));
  EXPECT_TRUE(exp(QuantityF<Dimensionless>{1.0F}).isApprox(QuantityF<Dimensionless>{std::exp(1.0F)}, 1e-6F));
}

TEST(TestQuantity, AbsoluteValue) {  // NOLINT
  const LengthQuantity length = abs(LengthQuantity{-2.5});
  EXPECT_EQ(length, LengthQuantity{2.5});
  EXPECT_EQ(abs(LengthQuantity{2.5}), LengthQuantity{2.5});
  EXPECT_EQ(abs(LengthQuantityI{-3}), LengthQuantityI{3});
  EXPECT_EQ(abs(QuantityF<Time>{-1.5F}), QuantityF<Time>{1.5F});
  // Unsigned values are their own absolute values: std::abs of them is ambiguous.
  using LengthQuantityU = Quantity<ValueMatrix<unsigned, 1, 1>, Length>;
  EXPECT_EQ(abs(LengthQuantityU{3U}), LengthQuantityU{3U});
}

// The square root and the cube root are pow<1, 2>() and pow<1, 3>(), computed with std::sqrt and std::cbrt.
TEST(TestQuantity, SquareAndCubeRoot) {  // NOLINT
  const LengthQuantity side = sqrt(QuantityD<Area>{2.0});
  EXPECT_EQ(side, LengthQuantity{std::sqrt(2.0)});
  // The cube roots are compared approximately: the compiler may compute std::cbrt(2.0) at compile time, correctly rounded, while
  // the cube root of the standard library may be one unit in the last place away.
  const LengthQuantity edge = cbrt(QuantityD<Volume>{2.0});
  EXPECT_TRUE(edge.isApprox(LengthQuantity{std::cbrt(2.0)}, 1e-15));
  EXPECT_TRUE(cbrt(QuantityD<Volume>{-2.0}).isApprox(LengthQuantity{-std::cbrt(2.0)}, 1e-15));
  EXPECT_TRUE(cbrt(QuantityD<Volume>{64.0}).isApprox(LengthQuantity{4.0}, 1e-15));
  EXPECT_TRUE(std::isnan(sqrt(QuantityD<Area>{-4.0}).getValue()));
  EXPECT_EQ(sqrt(QuantityF<Area>{4.0F}), QuantityF<Length>{2.0F});
  // Integer values are truncated, and integer values without a real square root throw.
  EXPECT_EQ(sqrt(AreaQuantityI{5}), LengthQuantityI{2});
  EXPECT_EQ(cbrt(QuantityI<Volume>{-9}), LengthQuantityI{-2});
  EXPECT_THROW(sqrt(AreaQuantityI{-4}), std::domain_error);
}

// The length of a vector of two components with the same unit, like std::hypot, also where the squares would overflow.
TEST(TestQuantity, Hypot) {  // NOLINT
  const LengthQuantity length = hypot(LengthQuantity{3.0}, LengthQuantity{4.0});
  EXPECT_EQ(length, LengthQuantity{5.0});
  EXPECT_TRUE((hypot(LengthQuantity{3e200}, LengthQuantity{4e200}) / LengthQuantity{5e200}).isApprox(DimensionlessQuantity{1.0}, 1e-12));
  EXPECT_TRUE(hypot(QuantityF<Velocity>{1.0F}, QuantityF<Velocity>{1.0F}).isApprox(QuantityF<Velocity>{std::sqrt(2.0F)}, 1e-6F));
  // Integer values are truncated exactly, like the powers: the hypotenuse of 2 10^8 and 2 10^4 is the square root of
  // (2 10^8 + 1)^2 - 1, which std::hypot rounds to 2 10^8 + 1.
  EXPECT_EQ(hypot(LengthQuantityI{3}, LengthQuantityI{-4}), LengthQuantityI{5});
  EXPECT_EQ(hypot(LengthQuantityI{1}, LengthQuantityI{1}), LengthQuantityI{1});
  EXPECT_EQ(hypot(LengthQuantityI{200'000'000}, LengthQuantityI{20'000}), LengthQuantityI{200'000'000});
  EXPECT_THROW(hypot(LengthQuantityI{std::numeric_limits<int>::max()}, LengthQuantityI{std::numeric_limits<int>::max()}), std::range_error);
  // x^2 + y^2 = 2^64 - 2^33 + 2 fits into 64 bits, but (root + 1)^2 = 2^64 of the root 2^32 - 1 does not.
  EXPECT_EQ(hypot(LengthQuantityLL{4'294'967'295LL}, LengthQuantityLL{1LL}), LengthQuantityLL{4'294'967'295LL});
  // If x^2, y^2 or their sum does not fit into 64 bits, the floating point hypotenuse is truncated, here 5 2^32 and 5 10^9 exactly.
  EXPECT_EQ(hypot(LengthQuantityLL{3LL << 32}, LengthQuantityLL{4LL << 32}), LengthQuantityLL{5LL << 32});
  EXPECT_EQ(hypot(LengthQuantityLL{0LL}, LengthQuantityLL{3LL << 32}), LengthQuantityLL{3LL << 32});
  EXPECT_EQ(hypot(LengthQuantityLL{3'000'000'000LL}, LengthQuantityLL{4'000'000'000LL}), LengthQuantityLL{5'000'000'000LL});
  // The hypotenuse of 64-bit unsigned values can be 2^64 or more, which no integer value type represents.
  const unsigned long long maximum = std::numeric_limits<unsigned long long>::max();
  EXPECT_THROW(hypot(LengthQuantityULL{maximum}, LengthQuantityULL{maximum}), std::range_error);
  EXPECT_EQ(hypot(LengthQuantityULL{3ULL << 32}, LengthQuantityULL{4ULL << 32}), LengthQuantityULL{5ULL << 32});
}

// The smaller and the larger of two quantities with the same unit, like std::min and std::max: the left-hand side if neither is
// smaller or larger, e.g. if one is NaN.
TEST(TestQuantity, MinAndMax) {  // NOLINT
  const LengthQuantity shorter = min(LengthQuantity{1.0}, LengthQuantity{2.0});
  const LengthQuantity longer = max(LengthQuantity{1.0}, LengthQuantity{2.0});
  EXPECT_EQ(shorter, LengthQuantity{1.0});
  EXPECT_EQ(longer, LengthQuantity{2.0});
  EXPECT_EQ(min(LengthQuantity{2.0}, LengthQuantity{1.0}), LengthQuantity{1.0});
  EXPECT_EQ(max(LengthQuantity{2.0}, LengthQuantity{1.0}), LengthQuantity{2.0});
  EXPECT_EQ(min(LengthQuantityI{-1}, LengthQuantityI{2}), LengthQuantityI{-1});
  EXPECT_EQ(max(LengthQuantityI{-1}, LengthQuantityI{2}), LengthQuantityI{2});
  const double nan = std::numeric_limits<double>::quiet_NaN();
  EXPECT_TRUE(std::isnan(min(LengthQuantity{nan}, LengthQuantity{1.0}).getValue()));
  EXPECT_TRUE(std::isnan(max(LengthQuantity{nan}, LengthQuantity{1.0}).getValue()));
  EXPECT_EQ(min(LengthQuantity{1.0}, LengthQuantity{nan}), LengthQuantity{1.0});
  EXPECT_EQ(max(LengthQuantity{1.0}, LengthQuantity{nan}), LengthQuantity{1.0});
}

// Quantities with the same unit but different types, e.g. a quantity and the unevaluated values of a sum, take the overloads of
// min() and max() for different types. With the archetype, whose sums are evaluated, the types are the same.
TEST(TestQuantity, MinAndMaxOfUnevaluatedValues) {  // NOLINT
  using Implementation = ValueD::ImplementationT;
  using UnevaluatedValues = Sum<ValueD, ValueD>::type;
  const ValueD one{1.0};
  const Quantity<UnevaluatedValues, Length> unevaluated{
      UnevaluatedValues{Sum<AccessImplementation<Implementation>, AccessImplementation<Implementation>>::value(one.getImplementation(),
                                                                                                               one.getImplementation())}};
  EXPECT_EQ(min(unevaluated, LengthQuantity{3.0}), LengthQuantity{2.0});
  EXPECT_EQ(max(LengthQuantity{3.0}, unevaluated), LengthQuantity{3.0});
}

// The powers of integer values that are not integers are truncated, like the norm of integer vectors (#63).
TEST(TestQuantity, PowerOfIntegers) {  // NOLINT
  EXPECT_EQ((pow<1, 2>(AreaQuantityI{4})), LengthQuantityI{2});
  EXPECT_EQ((pow<1, 2>(AreaQuantityI{5})), LengthQuantityI{2});
  EXPECT_EQ(pow<2>(LengthQuantityI{3}), AreaQuantityI{9});
  EXPECT_EQ(pow<-1>(QuantityI<Dimensionless>{2}), QuantityI<Dimensionless>{0});
}

// The powers of integers are truncated exactly, although they are estimated in floating point: e.g. 8^(2/3) is
// 3.9999999999999996 with std::pow, since the exponent 2/3 is not exact, and was truncated to 3.
TEST(TestQuantity, PowerOfIntegersWithIntegerResults) {  // NOLINT
  EXPECT_EQ((pow<2, 3>(QuantityI<Volume>{8})), AreaQuantityI{4});
  EXPECT_EQ((pow<2, 3>(QuantityI<Volume>{1000})), AreaQuantityI{100});
  EXPECT_EQ((pow<2, 3>(QuantityI<Volume>{-8})), AreaQuantityI{4});
  EXPECT_EQ((pow<1, 3>(QuantityI<Volume>{64})), LengthQuantityI{4});
  EXPECT_EQ((pow<1, 3>(QuantityI<Volume>{-125})), LengthQuantityI{-5});
  EXPECT_EQ((pow<1, 3>(QuantityI<Volume>{1000000})), LengthQuantityI{100});
  EXPECT_EQ((pow<3, 2>(AreaQuantityI{100})), QuantityI<Volume>{1000});
  EXPECT_EQ((pow<1, 2>(AreaQuantityI{46340 * 46340 - 1})), LengthQuantityI{46339});
  EXPECT_EQ((pow<1, 2>(AreaQuantityI{46340 * 46340})), LengthQuantityI{46340});
  EXPECT_EQ((pow<1, 3>(QuantityI<Volume>{63})), LengthQuantityI{3});
  // A negative exponent: the powers of 1 and -1 are exact, the others below 1 in magnitude, and truncated to 0.
  EXPECT_EQ(pow<-1>(QuantityI<Dimensionless>{-2}), QuantityI<Dimensionless>{0});
  EXPECT_EQ((pow<-1, 3>(QuantityI<Dimensionless>{1})), QuantityI<Dimensionless>{1});
  EXPECT_EQ((pow<-1, 2>(QuantityI<Dimensionless>{1})), QuantityI<Dimensionless>{1});
  EXPECT_EQ((pow<-1, 2>(QuantityI<Dimensionless>{4})), QuantityI<Dimensionless>{0});
  // The boundaries of int: -2^31 fits, 2^31 does not.
  EXPECT_EQ(pow<31>(QuantityI<Dimensionless>{-2}), QuantityI<Dimensionless>{std::numeric_limits<int>::min()});
  EXPECT_EQ(pow<2>(LengthQuantityI{46340}), AreaQuantityI{46340 * 46340});
}

// With 64-bit integers, the floating point estimates of roots that are not integers are within rounding errors of an integer, or
// even rounded to one: e.g. the square root of 10^14 - 1 is within 5e-15 of 10^7, relative, and double rounds 10^18 - 1 to 10^18.
// The roots are truncated exactly anyway. Only if x^P does not fit into 64 bits, the floating point power is truncated, e.g. that
// of (2^40)^2 for pow<2, 3>().
TEST(TestQuantity, PowerOfWideIntegers) {  // NOLINT
  EXPECT_EQ(sqrt(AreaQuantityLL{99'999'999'999'999LL}), LengthQuantityLL{9'999'999LL});
  EXPECT_EQ(sqrt(AreaQuantityLL{100'000'000'000'000LL}), LengthQuantityLL{10'000'000LL});
  EXPECT_EQ(cbrt(VolumeQuantityLL{999'999'999'999'999LL}), LengthQuantityLL{99'999LL});
  EXPECT_EQ(sqrt(AreaQuantityLL{999'999'999'999'999'999LL}), LengthQuantityLL{999'999'999LL});
  EXPECT_EQ(sqrt(AreaQuantityLL{1'000'000'000'000'000'000LL}), LengthQuantityLL{1'000'000'000LL});
  // 2^(80/3) = 106528681.31.
  EXPECT_EQ((pow<2, 3>(VolumeQuantityLL{1LL << 40})), AreaQuantityLL{106'528'681LL});
  // (2^32 - 1)^2 fits into 64 bits, but the cube of its cube root plus one, 2642246^3, does not.
  EXPECT_EQ((pow<2, 3>(VolumeQuantityLL{4'294'967'295LL})), AreaQuantityLL{2'642'245LL});
  // (5 10^12)^3 does not fit into 64 bits, and its square root, 1.1 10^19, does not fit into long long.
  EXPECT_THROW((pow<3, 2>(AreaQuantityLL{5'000'000'000'000LL})), std::range_error);
}

// The minimum of a signed integer type, e.g. INT_MIN = -2^31, has a magnitude that the type cannot represent, but its powers are
// exact too.
TEST(TestQuantity, PowerOfMinimumIntegers) {  // NOLINT
  const int minimum = std::numeric_limits<int>::min();
  EXPECT_EQ(pow<1>(QuantityI<Dimensionless>{minimum}), QuantityI<Dimensionless>{minimum});
  // 2^31 = 1290.16^3 and 2^63 = (2^21)^3.
  EXPECT_EQ(cbrt(QuantityI<Volume>{minimum}), LengthQuantityI{-1290});
  EXPECT_EQ(cbrt(VolumeQuantityLL{std::numeric_limits<long long>::min()}), LengthQuantityLL{-(1LL << 21)});
  EXPECT_THROW(sqrt(AreaQuantityI{minimum}), std::domain_error);
  EXPECT_THROW(pow<2>(LengthQuantityI{minimum}), std::range_error);
  // -2^33 is below the minimum.
  EXPECT_THROW(pow<33>(QuantityI<Dimensionless>{-2}), std::range_error);
}

// The powers of unsigned integers are exact up to the maximum of 64 bits, 2^64 - 1: e.g. the estimate of its square root is 2^32,
// whose square does not fit into 64 bits, and is corrected to 2^32 - 1.
TEST(TestQuantity, PowerOfUnsignedIntegers) {  // NOLINT
  using LengthQuantityU = Quantity<ValueMatrix<unsigned, 1, 1>, Length>;
  using AreaQuantityU = Quantity<ValueMatrix<unsigned, 1, 1>, Area>;
  using AreaQuantityULL = Quantity<ValueMatrix<unsigned long long, 1, 1>, Area>;
  using VolumeQuantityULL = Quantity<ValueMatrix<unsigned long long, 1, 1>, Volume>;
  EXPECT_EQ(sqrt(AreaQuantityU{17U}), LengthQuantityU{4U});
  EXPECT_EQ(pow<2>(LengthQuantityU{65'535U}), AreaQuantityU{4'294'836'225U});
  EXPECT_THROW(pow<2>(LengthQuantityU{65'536U}), std::range_error);
  EXPECT_THROW(pow<-1>(Quantity<ValueMatrix<unsigned, 1, 1>, Dimensionless>{0U}), std::domain_error);
  EXPECT_EQ(sqrt(AreaQuantityULL{std::numeric_limits<unsigned long long>::max()}), LengthQuantityULL{4'294'967'295ULL});
  // The estimate of the square root of (2^32 - 1)^2 is exact, and the square of 2^32 does not fit into 64 bits.
  EXPECT_EQ(sqrt(AreaQuantityULL{18'446'744'065'119'617'025ULL}), LengthQuantityULL{4'294'967'295ULL});
  EXPECT_EQ(pow<2>(LengthQuantityULL{4'294'967'295ULL}), AreaQuantityULL{18'446'744'065'119'617'025ULL});
  // (2^32)^2 = 2^64 is the smallest square that does not fit.
  EXPECT_THROW(pow<2>(LengthQuantityULL{1ULL << 32}), std::range_error);
  // (2^42)^3 does not fit into 64 bits, but its square root, 2^63, fits into unsigned long long.
  EXPECT_EQ((pow<3, 2>(AreaQuantityULL{1ULL << 42})), VolumeQuantityULL{1ULL << 63});
}

// Integer powers that the value type cannot represent throw, rather than converting infinity or a value out of range into an
// integer, which is undefined behavior: e.g. 0^(-1), 10^40 and 2^31 with int values.
TEST(TestQuantity, PowerOfIntegersOutOfRange) {  // NOLINT
  EXPECT_THROW(pow<-1>(QuantityI<Dimensionless>{0}), std::domain_error);
  EXPECT_THROW((pow<-1, 3>(QuantityI<Dimensionless>{0})), std::domain_error);
  EXPECT_THROW((pow<-1, 2>(QuantityI<Dimensionless>{0})), std::domain_error);
  EXPECT_THROW(pow<40>(QuantityI<Dimensionless>{10}), std::range_error);
  EXPECT_THROW(pow<31>(QuantityI<Dimensionless>{2}), std::range_error);
  EXPECT_THROW(pow<2>(LengthQuantityI{65536}), std::range_error);
  // (4 10^6)^3 does not fit into 64 bits, and its square root, 8 10^9, does not fit into int.
  EXPECT_THROW((pow<3, 2>(AreaQuantityI{4'000'000})), std::range_error);
  // Floating point values give infinity, like std::pow.
  EXPECT_EQ(pow<-1>(DimensionlessQuantity{0.0}), DimensionlessQuantity{std::numeric_limits<double>::infinity()});
}

TEST(TestQuantity, PowerOfFloatingPointValues) {  // NOLINT
  EXPECT_TRUE((pow<1, 2>(QuantityD<Area>{2.0}).isApprox(LengthQuantity{std::sqrt(2.0)}, 1e-12)));
  EXPECT_TRUE((pow<1, 2>(QuantityF<Area>{2.0f}).isApprox(QuantityF<Length>{std::sqrt(2.0f)}, 1e-6f)));
}

// A negative value has a real power if the reduced denominator Q of the exponent P/Q is odd, (-1)^P |x|^(P/Q), like std::cbrt,
// where std::pow is NaN. If Q is even, it has none: floating point values give NaN, like std::pow, and integer values throw,
// rather than converting NaN into an integer, which is undefined behavior.
TEST(TestQuantity, PowerOfNegativeValues) {  // NOLINT
  using VolumeQuantity = QuantityD<Volume>;
  using AreaQuantity = QuantityD<Area>;
  // Odd Q, with an odd and an even P.
  EXPECT_TRUE((pow<1, 3>(VolumeQuantity{-8.0}).isApprox(LengthQuantity{-2.0}, 1e-12)));
  EXPECT_TRUE((pow<1, 3>(VolumeQuantity{8.0}).isApprox(LengthQuantity{2.0}, 1e-12)));
  EXPECT_TRUE((pow<2, 3>(VolumeQuantity{-8.0}).isApprox(AreaQuantity{4.0}, 1e-12)));
  EXPECT_TRUE((pow<-1, 3>(DimensionlessQuantity{-8.0}).isApprox(DimensionlessQuantity{-0.5}, 1e-12)));
  EXPECT_EQ(pow<3>(LengthQuantity{-2.0}), VolumeQuantity{-8.0});
  EXPECT_EQ((pow<1, 3>(QuantityI<Volume>{-8})), LengthQuantityI{-2});
  EXPECT_EQ((pow<1, 3>(QuantityI<Volume>{8})), LengthQuantityI{2});
  EXPECT_EQ((pow<2, 3>(QuantityI<Volume>{-1})), AreaQuantityI{1});
  EXPECT_EQ(pow<2>(LengthQuantityI{-3}), AreaQuantityI{9});
  EXPECT_TRUE((pow<1, 3>(QuantityF<Volume>{-8.0f}).isApprox(QuantityF<Length>{-2.0f}, 1e-6f)));
  // A negative P, odd and even: (-8)^(-2/3) = 1/4, (-1)^(-1/3) = -1 and (-1)^(-1) = -1.
  EXPECT_TRUE((pow<-2, 3>(DimensionlessQuantity{-8.0}).isApprox(DimensionlessQuantity{0.25}, 1e-12)));
  EXPECT_EQ((pow<-1, 3>(QuantityI<Dimensionless>{-1})), QuantityI<Dimensionless>{-1});
  EXPECT_EQ(pow<-1>(QuantityI<Dimensionless>{-1}), QuantityI<Dimensionless>{-1});
  // The exponent is reduced: 2/6 is 1/3, whose Q is odd.
  EXPECT_TRUE((pow<2, 6>(VolumeQuantity{-8.0}).isApprox(LengthQuantity{-2.0}, 1e-12)));
  // Integer results are truncated toward zero, like those of positive values: -(9^(1/3)) = -2.08 becomes -2.
  EXPECT_EQ((pow<1, 3>(QuantityI<Volume>{-9})), LengthQuantityI{-2});
  // Even Q, also with a negative P.
  EXPECT_TRUE(std::isnan((pow<1, 2>(AreaQuantity{-4.0}).getValue())));
  EXPECT_TRUE(std::isnan((pow<-1, 2>(DimensionlessQuantity{-4.0}).getValue())));
  EXPECT_TRUE((pow<1, 2>(AreaQuantity{4.0}).isApprox(LengthQuantity{2.0}, 1e-12)));
  EXPECT_THROW((pow<1, 2>(AreaQuantityI{-4})), std::domain_error);
  EXPECT_THROW((pow<3, 2>(AreaQuantityI{-4})), std::domain_error);
  EXPECT_THROW((pow<-1, 2>(QuantityI<Dimensionless>{-4})), std::domain_error);
  EXPECT_EQ((pow<1, 2>(AreaQuantityI{4})), LengthQuantityI{2});
}

// Zero is not negative, so its powers are those of std::pow: zero for every positive exponent, also for an even Q, and also for
// negative zero, which compares equal to zero and so is not NaN.
TEST(TestQuantity, PowerOfZero) {  // NOLINT
  EXPECT_EQ((pow<1, 3>(QuantityD<Volume>{0.0})), LengthQuantity{0.0});
  EXPECT_EQ((pow<1, 2>(QuantityD<Area>{0.0})), LengthQuantity{0.0});
  EXPECT_EQ((pow<1, 2>(QuantityD<Area>{-0.0})), LengthQuantity{0.0});
  EXPECT_EQ((pow<1, 3>(QuantityI<Volume>{0})), LengthQuantityI{0});
  EXPECT_EQ((pow<1, 2>(AreaQuantityI{0})), LengthQuantityI{0});
}

// Quantities with other units do not convert implicitly, so that type traits report the conversion as invalid (#70).
TEST(TestQuantity, ConversionsAreImplicitOnlyIfValid) {  // NOLINT
  static_assert(std::is_convertible_v<Sum<LengthQuantity, LengthQuantity>::type, LengthQuantity>);
  static_assert(!std::is_convertible_v<LengthQuantity, TimeQuantity>);
  static_assert(!std::convertible_to<LengthQuantity, TimeQuantity>);
}

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_MP_UNITS
// mp-units gives named units their own types, e.g. si::newton is not the type of kg m / s^2. The adapter normalizes them, so
// that they match the units wuzp computes (#65). The other units backends have no such named units.
TEST(TestQuantity, NamedUnitsOfMpUnits) {  // NOLINT
  using Newton = wuzp::Unit<UnitType<mp_units::si::newton>>;
  const QuantityD<Newton> force{1.0};
  const QuantityD<Newton> sum = force + QuantityD<Mass>{2.0} * QuantityD<Acceleration>{3.0};
  EXPECT_EQ(sum, QuantityD<Force>{7.0});
}
#endif
