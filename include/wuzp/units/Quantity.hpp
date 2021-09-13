#pragma once

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp/units/wuzp/Quantity.hpp"
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp/units/boost/Quantity.hpp"
#endif
#include "wuzp/units/QuantityTraits.hpp"
#include "wuzp/units/UnitTraits.hpp"

namespace wuzp {

// Exponent first, as value and unit can be derived automatically.
template <int Exponent, typename Value, typename Unit>
auto pow(const units::Quantity<Value, Unit>& quantity) {
  return units::QuantityTraits::Create<units::Quantity<Value, typename units::UnitTraits::Power<Unit, Exponent>::type>>::create(
      std::pow(units::QuantityTraits::GetValue<units::Quantity<Value, Unit>>::get(quantity), Exponent));
}

template <typename Value>
auto sin(const units::Quantity<Value, units::Angle>& angle) {
  return units::QuantityTraits::Create<units::Quantity<Value, units::Dimensionless>>::create(
      std::sin(units::QuantityTraits::GetValue<units::Quantity<Value, units::Angle>>::get(angle)));
}

template <typename Value>
auto cos(const units::Quantity<Value, units::Angle>& angle) {
  return units::QuantityTraits::Create<units::Quantity<Value, units::Dimensionless>>::create(
      std::cos(units::QuantityTraits::GetValue<units::Quantity<Value, units::Angle>>::get(angle)));
}

// TODO(remod): add more math functions.

}  // namespace wuzp
