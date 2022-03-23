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

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
struct Sum<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>> {
  static_assert(std::is_same<ValueLhsT_, ValueRhsT_>::value, "Value types do not match.");
  using type = Quantity<ValueLhsT_, typename Sum<UnitLhsT_, UnitRhsT_>::type>;
};

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
struct Difference<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>> {
  static_assert(std::is_same<ValueLhsT_, ValueRhsT_>::value, "Value types do not match.");
  using type = Quantity<ValueLhsT_, typename Difference<UnitLhsT_, UnitRhsT_>::type>;
};

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
struct Product<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>> {
  static_assert(std::is_same<ValueLhsT_, ValueRhsT_>::value, "Value types do not match.");
  using type = Quantity<ValueLhsT_, typename Product<UnitLhsT_, UnitRhsT_>::type>;
};

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
struct Division<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>> {
  static_assert(std::is_same<ValueLhsT_, ValueRhsT_>::value, "Value types do not match.");
  using type = Quantity<ValueLhsT_, typename Division<UnitLhsT_, UnitRhsT_>::type>;
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
template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto operator+(const Quantity<ValueLhsT_, UnitLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return typename Sum<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>>::type(lhs.getValue() + rhs.getValue());
}

template <typename Value, typename Unit>
auto operator-(const Quantity<Value, Unit>& rhs) {
  return Quantity<Value, Unit>(-rhs.getValue());
}
template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto operator-(const Quantity<ValueLhsT_, UnitLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return typename Difference<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>>::type(lhs.getValue() - rhs.getValue());
}

template <typename ValueLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto operator*(const ValueLhsT_ lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return Quantity<ValueLhsT_, typename AccessUnits<SpecializationDummy<ValueLhsT_>>::Dimensionless>(lhs) * rhs;
}
template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_>
auto operator*(const Quantity<ValueLhsT_, UnitLhsT_>& lhs, const ValueRhsT_ rhs) {
  return rhs * lhs;
}
template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto operator*(const Quantity<ValueLhsT_, UnitLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return typename Product<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>>::type(lhs.getValue() * rhs.getValue());
}

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto operator/(const Quantity<ValueLhsT_, UnitLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return typename Division<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>>::type(lhs.getValue() / rhs.getValue());
}

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
bool operator==(const Quantity<ValueLhsT_, UnitLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  static_assert(std::is_same<UnitLhsT_, UnitRhsT_>::value, "Units do not match.");
  return lhs.getValue() == rhs.getValue();
}

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
bool operator!=(const Quantity<ValueLhsT_, UnitLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
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
