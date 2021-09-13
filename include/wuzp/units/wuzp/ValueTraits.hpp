#pragma once

#include "wuzp/core/traits.hpp"

namespace wuzp {

/**
 * Traits for operations with values.
 */
struct ValueTraits {
  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename ValueLhs, typename ValueRhs>
  struct Sum<ValueLhs, ValueRhs> {
    // Design decision: Do not allow mixing types, require a manual cast.
    static_assert(std::is_same<ValueLhs, ValueRhs>::value, "Value type mismatch.");
    using type = ValueLhs;
  };

  DEFINE_RECURSIVE_TRAIT(Difference)
  template <typename ValueLhs, typename ValueRhs>
  struct Difference<ValueLhs, ValueRhs> {
    // Design decision: Do not allow mixing types, require a manual cast.
    static_assert(std::is_same<ValueLhs, ValueRhs>::value, "Value type mismatch.");
    using type = ValueLhs;
  };

  DEFINE_RECURSIVE_TRAIT(Product)
  template <typename ValueLhs, typename ValueRhs>
  struct Product<ValueLhs, ValueRhs> {
    // Design decision: Do not allow mixing types, require a manual cast.
    static_assert(std::is_same<ValueLhs, ValueRhs>::value, "Value type mismatch.");
    using type = ValueLhs;
  };

  DEFINE_RECURSIVE_TRAIT(Division)
  template <typename ValueLhs, typename ValueRhs>
  struct Division<ValueLhs, ValueRhs> {
    // Design decision: Do not allow mixing types, require a manual cast.
    static_assert(std::is_same<ValueLhs, ValueRhs>::value, "Value type mismatch.");
    using type = ValueLhs;
  };

  template <typename T>
  struct Identity;
};

// TODO(remod): Add more types.
template <>
struct ValueTraits::Identity<int> {
  static constexpr int value_{1};
};
template <>
struct ValueTraits::Identity<float> {
  static constexpr float value_{1.f};
};
template <>
struct ValueTraits::Identity<double> {
  static constexpr double value_{1.0};
};

}  // namespace wuzp
