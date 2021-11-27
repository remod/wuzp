#pragma once

#include <utility>

#include <boost/units/quantity.hpp>

#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/traits.hpp"

#include "wuzp_boost/Unit.hpp"

namespace wuzp_boost {

// Convenience typedef to construct a quantity from the implementation.
template <typename ImplementationT_>
using QuantityFromImplementation = wuzp::Quantity<typename ImplementationT_::value_type, wuzp::Unit<typename ImplementationT_::unit_type>>;

}  // namespace wuzp_boost

namespace wuzp {

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Sum<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = wuzp_boost::QuantityFromImplementation<decltype(std::declval<Quantity<ValueLhs, UnitLhs>>().getImplementation() +
                                                               std::declval<Quantity<ValueRhs, UnitRhs>>().getImplementation())>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Difference<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = wuzp_boost::QuantityFromImplementation<decltype(std::declval<Quantity<ValueLhs, UnitLhs>>().getImplementation() -
                                                               std::declval<Quantity<ValueRhs, UnitRhs>>().getImplementation())>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Product<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = wuzp_boost::QuantityFromImplementation<decltype(std::declval<Quantity<ValueLhs, UnitLhs>>().getImplementation() *
                                                               std::declval<Quantity<ValueRhs, UnitRhs>>().getImplementation())>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Division<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
  using type = wuzp_boost::QuantityFromImplementation<decltype(std::declval<Quantity<ValueLhs, UnitLhs>>().getImplementation() /
                                                               std::declval<Quantity<ValueRhs, UnitRhs>>().getImplementation())>;
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
  using type = boost::units::quantity<Unit, Value>;
};

template <typename Value, typename Unit>
auto operator+(const Quantity<Value, Unit>& quantity) {
  return quantity;
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator+(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Sum<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() + rhs.getValue());
}

template <typename Value, typename Unit>
auto operator-(const Quantity<Value, Unit>& quantity) {
  return Quantity<Value, Unit>(-quantity.getValue());
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator-(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename Difference<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() - rhs.getValue());
}

template <typename ValueLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const ValueLhs lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return Quantity<ValueLhs, wuzp_boost::Dimensionless>(lhs) * rhs;
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const ValueRhs rhs) {
  return rhs * lhs;
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator*(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return
      typename Product<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getImplementation() * rhs.getImplementation());
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return
      typename Division<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getImplementation() / rhs.getImplementation());
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
bool operator==(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  static_assert(std::is_same<UnitLhs, UnitRhs>::value, "Units do not match.");
  return lhs.getImplementation() == rhs.getImplementation();
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

template <typename ValueT, typename UnitT>
void setQuantityValue(Quantity<ValueT, UnitT>& quantity, const ValueT& value) {
  quantity.setImplementation(Quantity<ValueT, UnitT>::ImplementationT::from_value(value));
}

template <typename ValueT, typename UnitT>
ValueT getQuantityValue(const Quantity<ValueT, UnitT>& quantity) {
  return quantity.getImplementation().value();
}

// Exponent first, as value and unit can be derived automatically.
template <int Exponent, typename Value, typename Unit>
auto pow(const Quantity<Value, Unit>& quantity) {
  return Quantity<Value, typename Power<Unit, Exponent>::type>(std::pow(quantity.getValue(), Exponent));
}

template <typename Value>
auto sin(const Quantity<Value, wuzp_boost::Angle>& angle) {
  return Quantity<Value, wuzp_boost::Dimensionless>(std::sin(angle.getValue()));
}

template <typename Value>
auto cos(const Quantity<Value, wuzp_boost::Angle>& angle) {
  return Quantity<Value, wuzp_boost::Dimensionless>(std::cos(angle.getValue()));
}

// TODO(remod): add more math functions.

}  // namespace wuzp
