#include <cmath>
#include <concepts>
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

TEST(TestQuantity, Chain) {  // NOLINT
  EXPECT_EQ(TimeQuantity{1.0} + TimeQuantity{2.0} * DimensionlessQuantity{3.0}, TimeQuantity{7.0});
}

#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestQuantity, Inverse) {  // NOLINT
  EXPECT_EQ(TimeQuantity{2.0}.inverse(), FrequencyQuantity{0.5});
}
#endif

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
