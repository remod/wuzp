#include <cmath>
#include <concepts>
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

// The sine and the cosine are computed in the value type, also for float. Compile tests Quantity.SinOfIntegerAngle and
// Quantity.CosOfIntegerAngle check that integer values are rejected.
TEST(TestQuantity, SineAndCosineOfFloats) {  // NOLINT
  const QuantityF<Angle> angle{0.5F};
  const QuantityF<Dimensionless> sine = sin(angle);
  const QuantityF<Dimensionless> cosine = cos(angle);
  EXPECT_TRUE(sine.isApprox(QuantityF<Dimensionless>{std::sin(0.5F)}, 1e-6F));
  EXPECT_TRUE(cosine.isApprox(QuantityF<Dimensionless>{std::cos(0.5F)}, 1e-6F));
}

// Powers are computed in floating point, also for integer values, and converted to the value type, which truncates like the
// norm of integer vectors (#63).
TEST(TestQuantity, PowerOfIntegers) {  // NOLINT
  EXPECT_EQ((pow<1, 2>(AreaQuantityI{4})), LengthQuantityI{2});
  EXPECT_EQ((pow<1, 2>(AreaQuantityI{5})), LengthQuantityI{2});
  EXPECT_EQ(pow<2>(LengthQuantityI{3}), AreaQuantityI{9});
  EXPECT_EQ(pow<-1>(QuantityI<Dimensionless>{2}), QuantityI<Dimensionless>{0});
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
