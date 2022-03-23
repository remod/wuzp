#pragma once

#include <ostream>

#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/traits.hpp"

#include "wuzp_units/Unit.hpp"

namespace wuzp_units {

template <typename ValueT_, typename UnitT_>
class QuantityImpl {
 public:
  using ValueT = ValueT_;
  using UnitT = UnitT_;
  using ThisT = QuantityImpl<ValueT, UnitT>;

  QuantityImpl() = default;
  template <typename ValueOtherT>
  explicit QuantityImpl(const ValueOtherT value) : value_(value) {
    // Design decision: Do not allow mixing types, require a manual cast.
    static_assert(std::is_same<ValueT, ValueOtherT>::value, "Values do not match.");
  }

  template <typename ValueOtherT>
  ThisT& setValue(const ValueOtherT value) {
    // Design decision: Do not allow mixing types, require a manual cast.
    static_assert(std::is_same<ValueT, ValueOtherT>::value, "Values do not match.");
    value_ = value;
    return *this;
  }
  ValueT getValue() const { return value_; }

  template <typename TargetValueT>
  QuantityImpl<TargetValueT, UnitT> cast() const {
    return QuantityImpl<TargetValueT, UnitT>(value_);
  }

  template <typename ValueOtherT, typename UnitOtherT>
  bool isApprox(const QuantityImpl<ValueOtherT, UnitOtherT>& other) const {
    // Note: Allow other value types to support lazy evaluation.
    static_assert(std::is_same<UnitT, UnitOtherT>::value, "Units do not match.");
    return value_.isApprox(other.getValue());
  }

 private:
  ValueT value_;
};

// Convenience typedef to construct a quantity from the implementation.
template <typename ImplementationT_>
using QuantityFromImplementation = wuzp::Quantity<typename ImplementationT_::ValueT, wuzp::Unit<typename ImplementationT_::UnitT>>;

}  // namespace wuzp_units

namespace wuzp {

// TODO(remod): Add more types.
template <>
struct Identity<int> {
  static constexpr int value_{1};
};
template <>
struct Identity<float> {
  static constexpr float value_{1.f};
};
template <>
struct Identity<double> {
  static constexpr double value_{1.0};
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Sum<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
  static_assert(std::is_same<ValueLhs, ValueRhs>::value, "Value types do not match.");
  using type = Quantity<ValueLhs, typename Sum<UnitLhs, UnitRhs>::type>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Difference<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
  static_assert(std::is_same<ValueLhs, ValueRhs>::value, "Value types do not match.");
  using type = Quantity<ValueLhs, typename Difference<UnitLhs, UnitRhs>::type>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
  static_assert(std::is_same<ValueLhs, ValueRhs>::value, "Value types do not match.");
  using type = Quantity<ValueLhs, typename Product<UnitLhs, UnitRhs>::type>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Division<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
  static_assert(std::is_same<ValueLhs, ValueRhs>::value, "Value types do not match.");
  using type = Quantity<ValueLhs, typename Division<UnitLhs, UnitRhs>::type>;
};

template <typename Value, typename Unit>
struct Identity<Quantity<Value, Unit>> {
  static const Quantity<Value, typename Identity<Unit>::type> value_{Identity<Value>::value_};
};

template <typename Value, typename Unit>
struct Inverse<Quantity<Value, Unit>> {
  using type = Quantity<Value, typename Inverse<Unit>::type>;
};

template <typename Value, typename Unit>
struct IsAbsolute<Quantity<Value, Unit>> {
  static constexpr bool value = IsAbsolute<Unit>::value;
};

template <typename Value, typename Unit>
struct MakeRelative<Quantity<Value, Unit>> {
  using type = Quantity<Value, typename MakeRelative<Unit>::type>;
  static type make(const Quantity<Value, Unit>& object) { return type(object.getValue()); }
};

template <typename Value, typename Unit>
struct MakeAbsolute<Quantity<Value, Unit>> {
  using type = Quantity<Value, typename MakeAbsolute<Unit>::type>;
  static type make(const Quantity<Value, Unit>& object) { return type(object.getValue()); }
};

template <typename ValueT, typename UnitImplementationT>
struct GetQuantityImplementation<ValueT, Unit<UnitImplementationT>> {
  template <typename Value, typename Unit>
  using type = wuzp_units::QuantityImpl<Value, Unit>;
};

template <typename Value, typename Unit>
auto operator+(const Quantity<Value, Unit>& rhs) {
  return rhs;
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator+(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Sum<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() + rhs.getValue());
}

template <typename Value, typename Unit>
auto operator-(const Quantity<Value, Unit>& rhs) {
  return Quantity<Value, Unit>(-rhs.getValue());
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator-(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Difference<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() - rhs.getValue());
}

template <typename ValueLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const ValueLhs lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return Quantity<ValueLhs, typename AccessUnits<SpecializationDummy<ValueLhs>>::Dimensionless>(lhs) * rhs;
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const ValueRhs rhs) {
  return rhs * lhs;
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Product<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() * rhs.getValue());
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Division<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() / rhs.getValue());
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
std::ostream& operator<<(std::ostream& lhs, const Quantity<Value, Unit>& rhs) {
  lhs << rhs.getValue();
  return lhs;
}

template <typename ValueT, typename UnitT>
void setQuantityValue(Quantity<ValueT, UnitT>& quantity, const ValueT& value) {
  quantity.setImplementation(quantity.getImplementation().setValue(value));
}

template <typename ValueT, typename UnitT>
ValueT getQuantityValue(const Quantity<ValueT, UnitT>& quantity) {
  return quantity.getImplementation().getValue();
}

}  // namespace wuzp
