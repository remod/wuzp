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

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
struct Sum<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>> {
  using type = wuzp_boost::QuantityFromImplementation<decltype(std::declval<Quantity<ValueLhsT_, UnitLhsT_>>().getImplementation() +
                                                               std::declval<Quantity<ValueRhsT_, UnitRhsT_>>().getImplementation())>;
};

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
struct Difference<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>> {
  using type = wuzp_boost::QuantityFromImplementation<decltype(std::declval<Quantity<ValueLhsT_, UnitLhsT_>>().getImplementation() -
                                                               std::declval<Quantity<ValueRhsT_, UnitRhsT_>>().getImplementation())>;
};

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
struct Product<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>> {
  using type = wuzp_boost::QuantityFromImplementation<decltype(std::declval<Quantity<ValueLhsT_, UnitLhsT_>>().getImplementation() *
                                                               std::declval<Quantity<ValueRhsT_, UnitRhsT_>>().getImplementation())>;
};

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
struct Division<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>> {
  using type = wuzp_boost::QuantityFromImplementation<decltype(std::declval<Quantity<ValueLhsT_, UnitLhsT_>>().getImplementation() /
                                                               std::declval<Quantity<ValueRhsT_, UnitRhsT_>>().getImplementation())>;
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
  return Quantity<ValueLhsT_, wuzp_boost::Dimensionless>(lhs) * rhs;
}
template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_>
auto operator*(const Quantity<ValueLhsT_, UnitLhsT_>& lhs, const ValueRhsT_ rhs) {
  return rhs * lhs;
}
template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto operator*(const Quantity<ValueLhsT_, UnitLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return typename Product<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>>::type(lhs.getImplementation() *
                                                                                                  rhs.getImplementation());
}

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
auto operator/(const Quantity<ValueLhsT_, UnitLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  return typename Division<Quantity<ValueLhsT_, UnitLhsT_>, Quantity<ValueRhsT_, UnitRhsT_>>::type(lhs.getImplementation() /
                                                                                                   rhs.getImplementation());
}

template <typename ValueLhsT_, typename UnitLhsT_, typename ValueRhsT_, typename UnitRhsT_>
bool operator==(const Quantity<ValueLhsT_, UnitLhsT_>& lhs, const Quantity<ValueRhsT_, UnitRhsT_>& rhs) {
  static_assert(std::is_same<UnitLhsT_, UnitRhsT_>::value, "Units do not match.");
  return lhs.getImplementation() == rhs.getImplementation();
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
  quantity.setImplementation(Quantity<ValueT, UnitT>::ImplementationT::from_value(value));
}

template <typename ValueT, typename UnitT>
ValueT getQuantityValue(const Quantity<ValueT, UnitT>& quantity) {
  return quantity.getImplementation().value();
}

}  // namespace wuzp
