#pragma once

#include "ctm/core/traits.hpp"
#include "ctm/units/ctm/UnitTraits.hpp"

namespace ctm {
namespace units {

template <typename Value, typename Unit>
class Quantity;

struct QuantityTraits {
  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
  struct Sum<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
    using type = Quantity<typename ValueSum<ValueLhs, ValueRhs>::type, typename units::UnitTraits::template Sum<UnitLhs, UnitRhs>::type>;
  };

  DEFINE_RECURSIVE_TRAIT(Difference)
  template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
  struct Difference<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
    using type = Quantity<typename ValueDifference<ValueLhs, ValueRhs>::type,
    typename units::UnitTraits::template Difference<UnitLhs, UnitRhs>::type>;
  };

  DEFINE_RECURSIVE_TRAIT(Product)
  template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
  struct Product<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
    using type =
        Quantity<typename ValueProduct<ValueLhs, ValueRhs>::type, typename units::UnitTraits::template Product<UnitLhs, UnitRhs>::type>;
  };

  DEFINE_RECURSIVE_TRAIT(Division)
  template <typename ValueLhs, typename UnitLhs, typename ValueRhs, typename UnitRhs>
  struct Division<Quantity<ValueLhs, UnitLhs>, Quantity<ValueRhs, UnitRhs>> {
    using type =
        Quantity<typename ValueDivision<ValueLhs, ValueRhs>::type, typename units::UnitTraits::template Division<UnitLhs, UnitRhs>::type>;
  };

  template <typename Quantity>
  struct Identity;
  template <typename Value, typename Unit>
  struct Identity<Quantity<Value, Unit>> {
    static const Quantity<Value, typename units::UnitTraits::template Identity<Unit>::type> value_{ValueIdentity<Value>::value_};
  };

  template <typename Quantity>
  struct Inverse;
  template <typename Value, typename Unit>
  struct Inverse<Quantity<Value, Unit>> {
    using type = Quantity<Value, typename units::UnitTraits::template Inverse<Unit>::type>;
  };
};

}
}  // namespace ctm
