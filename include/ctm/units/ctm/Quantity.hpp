#pragma once

#include <ostream>

#include "ctm/units/ctm/QuantityTraits.hpp"

namespace ctm {
namespace units {

template <typename Value, typename Unit>
class Quantity {
 public:
  Quantity() = default;
  explicit Quantity(Value value) : value_(value) {}

  void setValue(Value value) { value_ = value; }
  Value getValue() const { return value_; }

  template <typename TargetValue>
  Quantity<TargetValue, Unit> cast() const {
    return Quantity<TargetValue, Unit>(value_);
  }

  template <typename ValueOther, typename UnitOther>
  bool isApprox(const Quantity<ValueOther, UnitOther>& other) const {
    return value_.isApprox(other.getValue());
  }

  auto inverse() const { return typename QuantityTraits::Inverse<This>::type(ValueIdentity<Value>::value_ / value_); }

 private:
  using This = Quantity<Value, Unit>;

  Value value_;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator+(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename QuantityTraits::Sum<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() + rhs.getValue());
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator-(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename QuantityTraits::Difference<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() - rhs.getValue());
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename QuantityTraits::Product<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() * rhs.getValue());
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename QuantityTraits::Division<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() / rhs.getValue());
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
bool operator==(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  static_assert(std::is_same<UnitLhs, UnitRhs>::value, "Unit mismatch.");
  return lhs.getValue() == rhs.getValue();
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
bool operator!=(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return !(lhs == rhs);
}

template <typename Value, typename Unit>
std::ostream& operator<<(std::ostream& stream, const Quantity<Value, Unit>& quantity) {
  stream << quantity.getValue();
  return stream;
}

}
}  // namespace ctm
