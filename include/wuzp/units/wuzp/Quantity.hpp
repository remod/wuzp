#pragma once

#include <cmath>
#include <ostream>

#include "wuzp/units/wuzp/QuantityTraits.hpp"
#include "wuzp/units/wuzp/Unit.hpp"
#include "wuzp/units/wuzp/ValueTraits.hpp"

namespace wuzp {

template <typename Value, typename Unit>
class Quantity {
 public:
  Quantity() = default;
  template <typename ValueOther>
  explicit Quantity(const ValueOther value) : value_(value) {
    // Design decision: Do not allow mixing types, require a manual cast.
    static_assert(std::is_same<Value, ValueOther>::value, "Values do not match.");
  }

  template <typename ValueOther>
  void setValue(const ValueOther value) {
    // Design decision: Do not allow mixing types, require a manual cast.
    static_assert(std::is_same<Value, ValueOther>::value, "Values do not match.");
    value_ = value;
  }
  Value getValue() const { return value_; }

  template <typename TargetValue>
  Quantity<TargetValue, Unit> cast() const {
    return Quantity<TargetValue, Unit>(value_);
  }

  template <typename ValueOther, typename UnitOther>
  bool isApprox(const Quantity<ValueOther, UnitOther>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<Unit, UnitOther>::value, "Units do not match.");
    return value_.isApprox(other.getValue());
  }

  auto inverse() const { return typename QuantityTraits::Inverse<This>::type(ValueTraits::Identity<Value>::value_ / value_); }

 private:
  using This = Quantity<Value, Unit>;

  Value value_;
};

template <typename Value, typename Unit>
auto operator+(const Quantity<Value, Unit>& quantity) {
  return quantity;
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator+(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename QuantityTraits::Sum<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() + rhs.getValue());
}

template <typename Value, typename Unit>
auto operator-(const Quantity<Value, Unit>& quantity) {
  return Quantity<Value, Unit>(-quantity.getValue());
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator-(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return
      typename QuantityTraits::Difference<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() - rhs.getValue());
}

template <typename ValueLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const ValueLhs lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return Quantity<ValueLhs, Dimensionless>(lhs) * rhs;
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const ValueRhs rhs) {
  return rhs * lhs;
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
  static_assert(std::is_same<UnitLhs, UnitRhs>::value, "Units do not match.");
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

}  // namespace wuzp
