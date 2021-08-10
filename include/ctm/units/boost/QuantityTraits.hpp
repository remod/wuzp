#pragma once

#include "ctm/units/boost/Quantity.hpp"

namespace ctm {
namespace units {

struct QuantityTraits {
  // TODO(remod): Could probably be simpler.
  template <typename Quantity>
  struct Create;
  template <typename Value, typename Unit>
  struct Create<Quantity<Value, Unit>> {
    static Quantity<Value, Unit> create(const Value value) { return boost::units::quantity<Unit, Value>::from_value(value); }
  };

  // TODO(remod): Could probably be simpler.
  template <typename Quantity>
  struct GetValue;
  template <typename Unit, typename Value>
  struct GetValue<boost::units::quantity<Unit, Value>> {
    static Value get(const boost::units::quantity<Unit, Value>& quantity) { return quantity.value(); }
  };

  template <typename Quantity>
  struct GetUnit;
  template <typename Value, typename Unit>
  struct GetUnit<Quantity<Value, Unit>> {
    using type = Unit;
  };
};

}  // namespace units
}  // namespace ctm
