#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"
#include "wuzp/units/Quantity.hpp"
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/units/Unit.hpp"

using namespace wuzp;  // NOLINT

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
template <typename Value>
auto createDimensionlessQuantity(const Value value) {
  return Quantity<Value, Dimensionless>(value);
}
template <typename Value>
auto createTimeQuantity(const Value value) {
  return Quantity<Value, Time>(value);
}
template <typename Value>
auto createFrequencyQuantity(const Value value) {
  return Quantity<Value, Frequency>(value);
}
template <typename Value, typename Unit>
void setValue(Quantity<Value, Unit>& quantity, const Value value) {
  quantity.setValue(value);
}
template <typename Value, typename Unit>
Value getValue(const Quantity<Value, Unit> quantity) {
  return quantity.getValue();
}
template <typename Value, typename Unit>
auto inverse(const Quantity<Value, Unit> quantity) {
  return quantity.inverse();
}
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
template <typename Value>
boost::units::quantity<boost::units::si::dimensionless, Value> createDimensionlessQuantity(const Value value) {
  return value;
}
template <typename Value>
boost::units::quantity<boost::units::si::time, Value> createTimeQuantity(const Value value) {
  return value * boost::units::si::second;
}
template <typename Value>
boost::units::quantity<boost::units::si::frequency, Value> createFrequencyQuantity(const Value value) {
  return value * boost::units::si::frequency{};
}
template <typename Value>
void setValue(boost::units::quantity<boost::units::si::time, Value>& quantity, const Value value) {
  quantity = value * boost::units::si::second;
}
template <typename Value>
Value getValue(const boost::units::quantity<boost::units::si::time, Value> quantity) {
  return quantity.value();
}
template <typename Value>
boost::units::quantity<boost::units::si::frequency, Value> inverse(const boost::units::quantity<boost::units::si::time, Value> quantity) {
  return createDimensionlessQuantity(1.0) / quantity;
}
#endif

TEST(TestQuantity, ValueAccess) {  // NOLINT
  auto time = createTimeQuantity(1.0);
  EXPECT_EQ(getValue(time), 1.0);
  setValue(time, 2.0);
  EXPECT_EQ(getValue(time), 2.0);
}

TEST(TestQuantity, Addition) {  // NOLINT
  EXPECT_EQ(createTimeQuantity(1.0) + createTimeQuantity(2.0), createTimeQuantity(3.0));
}

TEST(TestQuantity, Subtraction) {  // NOLINT
  EXPECT_EQ(createTimeQuantity(3.0) - createTimeQuantity(2.0), createTimeQuantity(1.0));
}

TEST(TestQuantity, EqualityOperator) {  // NOLINT
  EXPECT_EQ(createTimeQuantity(1.0), createTimeQuantity(1.0));
}

TEST(TestQuantity, InEqualityOperator) {  // NOLINT
  EXPECT_NE(createTimeQuantity(1.0), createTimeQuantity(2.0));
}

TEST(TestQuantity, Product) {  // NOLINT
  EXPECT_EQ(createTimeQuantity(2.0) * createFrequencyQuantity(3.0), createDimensionlessQuantity(6.0));
}

TEST(TestQuantity, Chain) {  // NOLINT
  EXPECT_EQ(createTimeQuantity(1.0) + createTimeQuantity(2.0) * createDimensionlessQuantity(3.0), createTimeQuantity(7.0));
}

TEST(TestQuantity, Inverse) {  // NOLINT
  EXPECT_EQ(inverse(createTimeQuantity(2.0)), createFrequencyQuantity(0.5));
}

TEST(TestQuantity, ImplicitConversion) {  // NOLINT
  const auto radius = QuantityTraits::Create<Quantity<double, Length>>::create(2.0);
  const auto angle = QuantityTraits::Create<Quantity<double, Angle>>::create(3.0);
  [[maybe_unused]] const Quantity<double, Length> arcLength =
      angle *
      radius
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
      // With boost, angles are not dimensionless. We need to "normalize" the result.
      / QuantityTraits::Create<Quantity<double, Angle>>::create(1.0)
#endif
      ;
}
