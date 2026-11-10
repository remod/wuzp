#pragma once

#include <type_traits>

#include "wuzp/core/traits.hpp"

namespace wuzp {

template <typename ImplementationT_>
struct Unit {
  using ImplementationT = ImplementationT_;
};

// The sum and the difference of quantities only exist if their units match. Every units backend gives equal units the same type
// (see UnitSystem in concepts.hpp), so the core compares them by their types, for every units backend.
template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Sum<Unit<ImplementationLhsT_>, Unit<ImplementationRhsT_>> {
  static_assert(std::is_same_v<ImplementationLhsT_, ImplementationRhsT_>, "Units do not match.");
  using type = Unit<ImplementationLhsT_>;
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Difference<Unit<ImplementationLhsT_>, Unit<ImplementationRhsT_>> {
  static_assert(std::is_same_v<ImplementationLhsT_, ImplementationRhsT_>, "Units do not match.");
  using type = Unit<ImplementationLhsT_>;
};

}  // namespace wuzp
