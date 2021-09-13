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
auto pow(const Quantity<Value, Unit>& quantity) {
  return QuantityTraits::Create<Quantity<Value, typename UnitTraits::Power<Unit, Exponent>::type>>::create(
      std::pow(QuantityTraits::GetValue<Quantity<Value, Unit>>::get(quantity), Exponent));
}

template <typename Value>
auto sin(const Quantity<Value, Angle>& angle) {
  return QuantityTraits::Create<Quantity<Value, Dimensionless>>::create(
      std::sin(QuantityTraits::GetValue<Quantity<Value, Angle>>::get(angle)));
}

template <typename Value>
auto cos(const Quantity<Value, Angle>& angle) {
  return QuantityTraits::Create<Quantity<Value, Dimensionless>>::create(
      std::cos(QuantityTraits::GetValue<Quantity<Value, Angle>>::get(angle)));
}

// TODO(remod): add more math functions.

}  // namespace wuzp
