#pragma once

#include <utility>

// Implementations of the elementwise arithmetic traits of a value matrix backend with the C++ operators of its library, e.g.
// `implLhs + implRhs` for Sum. A backend whose library provides these operators inherits from them in its specializations, which
// stay constrained to its own implementation types: e.g. wuzp_eigen's Sum for two EigenArrays inherits from OperatorSum. The type
// of a result is the type the operator returns, e.g. a lazy expression of the library, which the core evaluates.

namespace wuzp::internal {

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct OperatorSum {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) { return implLhs + implRhs; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct OperatorDifference {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) { return implLhs - implRhs; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <typename ImplementationT_>
struct OperatorNegation {
  static auto value(const ImplementationT_& impl) { return -impl; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

//! The elementwise product with a matrix of the same backend or with a scalar, RhsT_.
template <typename ImplementationLhsT_, typename RhsT_>
struct OperatorProduct {
  static auto value(const ImplementationLhsT_& implLhs, const RhsT_& rhs) { return implLhs * rhs; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const RhsT_>()));
};

//! The elementwise quotient by a matrix of the same backend or by a scalar, RhsT_.
template <typename ImplementationLhsT_, typename RhsT_>
struct OperatorDivision {
  static auto value(const ImplementationLhsT_& implLhs, const RhsT_& rhs) { return implLhs / rhs; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const RhsT_>()));
};

}  // namespace wuzp::internal
