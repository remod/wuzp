#pragma once

#include <cmath>
#include <ostream>

#include "wuzp/units/wuzp/QuantityTraits.hpp"
#include "wuzp/units/wuzp/Unit.hpp"
#include "wuzp/units/wuzp/ValueTraits.hpp"

namespace wuzp {

template <typename ValueT_, typename UnitT_>
class Quantity {
 public:
  using ValueT = ValueT_;
  using UnitT = UnitT_;
  using ThisT = Quantity<ValueT, UnitT>;

  Quantity() = default;
  template <typename ValueOtherT>
  explicit Quantity(const ValueOtherT value) : value_(value) {
    // Design decision: Do not allow mixing types, require a manual cast.
    static_assert(std::is_same<Value, ValueOther>::value, "Values do not match.");
  }

  template <typename ValueOtherT>
  void setValue(const ValueOtherT value) {
    // Design decision: Do not allow mixing types, require a manual cast.
    static_assert(std::is_same<Value, ValueOther>::value, "Values do not match.");
    value_ = value;
  }
  ValueT getValue() const { return value_; }

  template <typename TargetValueT>
  Quantity<TargetValueT, UnitT> cast() const {
    return Quantity<TargetValueT, UnitT>(value_);
  }

  template <typename ValueOtherT, typename UnitOtherT>
  bool isApprox(const Quantity<ValueOtherT, UnitOtherT>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<UnitT, UnitOtherT>::value, "Units do not match.");
    return value_.isApprox(other.getValue());
  }

  auto inverse() const { return typename QuantityTraits::Inverse<This>::type(ValueTraits::Identity<ValueT>::value_ / value_); }

 private:
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

namespace wuzp {

template <typename Value, typename Unit>
struct IsAbsolute<wuzp_boost::Quantity<Value, Unit>> {
  static constexpr bool value = IsAbsolute<Unit>::value;
};
template <typename Value, typename Unit>
struct MakeRelative<wuzp_boost::Quantity<Value, Unit>> {
  using type = wuzp_boost::Quantity<Value, typename MakeRelative<Unit>::type>;
  static type make(const wuzp_boost::Quantity<Value, Unit>& object) { return type(object.getValue()); }
};
template <typename Value, typename Unit>
struct MakeAbsolute<wuzp_boost::Quantity<Value, Unit>> {
  using type = wuzp_boost::Quantity<Value, typename MakeAbsolute<Unit>::type>;
  static type make(const wuzp_boost::Quantity<Value, Unit>& object) { return type(object.getValue()); }
};

// Exponent first, as value and unit can be derived automatically.
template <int Exponent, typename Value, typename Unit>
auto pow(const wuzp_boost::Quantity<Value, Unit>& quantity) {
  return Quantity<Value, typename Power<Unit, Exponent>::type>(std::pow(quantity.getValue(), Exponent));
}

template <typename Value>
auto sin(const wuzp_boost::Quantity<Value, wuzp_boost::Angle>& angle) {
  return Quantity<Value, wuzp_boost::Dimensionless>(std::sin(angle.getValue()));
}

template <typename Value>
auto cos(const wuzp_boost::Quantity<Value, wuzp_boost::Angle>& angle) {
  return Quantity<Value, wuzp_boost::Dimensionless>(std::cos(angle.getValue()));
}

// TODO(remod): add more math functions.

}  // namespace wuzp
