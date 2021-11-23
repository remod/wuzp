#pragma once

#include <utility>

#include <boost/units/quantity.hpp>

#include "wuzp/core/traits.hpp"
#include "wuzp/units/QuantityInterface.hpp"

#include "wuzp_boost/Unit.hpp"

namespace wuzp_boost {

template <typename ValueT_, typename UnitT_>
class Quantity : public wuzp::QuantityInterface<Quantity<ValueT_, UnitT_>, ValueT_> {
  friend class wuzp::QuantityInterface<Quantity<ValueT_, UnitT_>, ValueT_>;

 public:
  using ValueT = ValueT_;
  using UnitT = UnitT_;
  using ThisT = Quantity<ValueT, UnitT>;
  using ImplementationT = boost::units::quantity<typename UnitT::ImplementationT, ValueT>;

  Quantity() = default;
  explicit Quantity(const ImplementationT& implementation) : implementation_(implementation) {}
  explicit Quantity(ImplementationT&& implementation) : implementation_(std::move(implementation)) {}
  explicit Quantity(const ValueT& value) { setValueImpl(value); }

  const ImplementationT& getImplementation() const { return implementation_; }

 private:
  void setValueImpl(const ValueT& value) { implementation_ = implementation_.from_value(value); }
  const ValueT& getValueImpl() const { return implementation_.value(); }

  ImplementationT implementation_;
};

// Convenience typedef to construct a quantity from the implementation.
template <typename ImplementationT_>
using QuantityFromImplementation = Quantity<typename ImplementationT_::value_type, wuzp::Unit<typename ImplementationT_::unit_type>>;

}  // namespace wuzp_boost

namespace wuzp {

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Sum<wuzp_boost::Quantity<ValueLhs, UnitLhs>, wuzp_boost::Quantity<ValueRhs, UnitRhs>> {
  using type =
      wuzp_boost::QuantityFromImplementation<decltype(std::declval<wuzp_boost::Quantity<ValueLhs, UnitLhs>>().getImplementation() +
                                                      std::declval<wuzp_boost::Quantity<ValueRhs, UnitRhs>>().getImplementation())>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Difference<wuzp_boost::Quantity<ValueLhs, UnitLhs>, wuzp_boost::Quantity<ValueRhs, UnitRhs>> {
  using type =
      wuzp_boost::QuantityFromImplementation<decltype(std::declval<wuzp_boost::Quantity<ValueLhs, UnitLhs>>().getImplementation() -
                                                      std::declval<wuzp_boost::Quantity<ValueRhs, UnitRhs>>().getImplementation())>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Product<wuzp_boost::Quantity<ValueLhs, UnitLhs>, wuzp_boost::Quantity<ValueRhs, UnitRhs>> {
  using type =
      wuzp_boost::QuantityFromImplementation<decltype(std::declval<wuzp_boost::Quantity<ValueLhs, UnitLhs>>().getImplementation() *
                                                      std::declval<wuzp_boost::Quantity<ValueRhs, UnitRhs>>().getImplementation())>;
};

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
struct Division<wuzp_boost::Quantity<ValueLhs, UnitLhs>, wuzp_boost::Quantity<ValueRhs, UnitRhs>> {
  using type =
      wuzp_boost::QuantityFromImplementation<decltype(std::declval<wuzp_boost::Quantity<ValueLhs, UnitLhs>>().getImplementation() /
                                                      std::declval<wuzp_boost::Quantity<ValueRhs, UnitRhs>>().getImplementation())>;
};

template <typename Value, typename Unit>
struct Identity<wuzp_boost::Quantity<Value, Unit>> {
  static const wuzp_boost::Quantity<Value, typename Identity<Unit>::type> value_{Identity<Value>::value_};
};

template <typename Value, typename Unit>
struct Inverse<wuzp_boost::Quantity<Value, Unit>> {
  using type = wuzp_boost::Quantity<Value, typename Inverse<Unit>::type>;
};

template <typename ImplementationT_>
struct GetQuantity<wuzp::Unit<ImplementationT_>> {
  template <typename ValueT_, typename UnitT_>
  using type = wuzp_boost::Quantity<ValueT_, UnitT_>;
};

}  // namespace wuzp

namespace wuzp_boost {

template <typename Value, typename Unit>
auto operator+(const Quantity<Value, Unit>& quantity) {
  return quantity;
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator+(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename wuzp::Sum<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() + rhs.getValue());
}

template <typename Value, typename Unit>
auto operator-(const Quantity<Value, Unit>& quantity) {
  return Quantity<Value, Unit>(-quantity.getValue());
}
template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator-(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename wuzp::Difference<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() - rhs.getValue());
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
  return typename wuzp::Product<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() * rhs.getValue());
}

template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
auto operator/(const Quantity<ValueLhs, UnitLhs>& lhs, const Quantity<ValueRhs, UnitRhs>& rhs) {
  return typename wuzp::Division<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>>::type(lhs.getValue() / rhs.getValue());
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

}  // namespace wuzp_boost

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
