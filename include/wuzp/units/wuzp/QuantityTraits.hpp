#pragma once

#include "wuzp/core/traits.hpp"
#include "wuzp/units/wuzp/UnitTraits.hpp"
#include "wuzp/units/wuzp/ValueTraits.hpp"

namespace wuzp {

template <typename Value, typename Unit>
class Quantity;

struct QuantityTraits {
  // TODO(remod): Could probably be simpler.
  template <typename Quantity>
  struct Create;
  template <typename Value, typename Unit>
  struct Create<Quantity<Value, Unit>> {
    static Quantity<Value, Unit> create(const Value value) { return Quantity<Value, Unit>(value); }
  };

  // TODO(remod): Could probably be simpler.
  template <typename Quantity>
  struct GetValue;
  template <typename Value, typename Unit>
  struct GetValue<Quantity<Value, Unit>> {
    static Value get(const Quantity<Value, Unit>& quantity) { return quantity.getValue(); }
  };

  template <typename Quantity>
  struct GetUnit;
  template <typename Value, typename Unit>
  struct GetUnit<Quantity<Value, Unit>> {
    using type = Unit;
  };

  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
  struct Sum<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
    using type = Quantity<typename ValueTraits::Sum<ValueLhs, ValueRhs>::type, typename UnitTraits::template Sum<UnitLhs, UnitRhs>::type>;
  };

  DEFINE_RECURSIVE_TRAIT(Difference)
  template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
  struct Difference<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
    using type = Quantity<typename ValueTraits::Difference<ValueLhs, ValueRhs>::type,
                          typename UnitTraits::template Difference<UnitLhs, UnitRhs>::type>;
  };

  DEFINE_RECURSIVE_TRAIT(Product)
  template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
  struct Product<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
    using type =
        Quantity<typename ValueTraits::Product<ValueLhs, ValueRhs>::type, typename UnitTraits::template Product<UnitLhs, UnitRhs>::type>;
  };

  DEFINE_RECURSIVE_TRAIT(Division)
  template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
  struct Division<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
    using type =
        Quantity<typename ValueTraits::Division<ValueLhs, ValueRhs>::type, typename UnitTraits::template Division<UnitLhs, UnitRhs>::type>;
  };

  template <typename Quantity>
  struct Identity;
  template <typename Value, typename Unit>
  struct Identity<Quantity<Value, Unit>> {
    static const Quantity<Value, typename UnitTraits::template Identity<Unit>::type> value_{ValueTraits::Identity<Value>::value_};
  };

  template <typename Quantity>
  struct Inverse;
  template <typename Value, typename Unit>
  struct Inverse<Quantity<Value, Unit>> {
    using type = Quantity<Value, typename UnitTraits::template Inverse<Unit>::type>;
  };
};

}  // namespace wuzp
