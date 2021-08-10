#include <gtest/gtest.h>

#include "ctm/core/traits.hpp"
#include "ctm/units/Quantity.hpp"
#include "ctm/units/Unit.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_CTM
template <typename Value>
auto createDimensionlessQuantity(const Value value) {
  return ctm::units::Quantity<Value, ctm::units::Dimensionless>(value);
}
template <typename Value>
auto createTimeQuantity(const Value value) {
  return ctm::units::Quantity<Value, ctm::units::Time>(value);
}
template <typename Value>
auto createFrequencyQuantity(const Value value) {
  return ctm::units::Quantity<Value, ctm::units::Frequency>(value);
}
template <typename Value, typename Unit>
void setValue(ctm::units::Quantity<Value, Unit>& quantity, const Value value) {
  quantity.setValue(value);
}
template <typename Value, typename Unit>
Value getValue(const ctm::units::Quantity<Value, Unit> quantity) {
  return quantity.getValue();
}
template <typename Value, typename Unit>
auto inverse(const ctm::units::Quantity<Value, Unit> quantity) {
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
  return value * boost::units::si::hertz;
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
