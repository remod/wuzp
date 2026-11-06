#pragma once

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {
namespace internal {

//! Applies a map of relative tensors, e.g. a linear map, to a tensor. An absolute tensor (e.g. a point) is mapped through its
//! coordinates relative to the origin, and the result is absolute again: f(p) = o + f(p - o). The map gets a temporary, so it
//! must return an evaluated result.
//! These are overloads rather than one function with `if constexpr`: MSVC compiles both branches of an `if constexpr` in
//! member functions of class templates, and the branch for relative tensors does not compile for absolute ones.
template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_, typename MapT_>
auto mapRelative(const Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_, Relative>& object,
                 const MapT_& map) {
  return map(object);
}
template <int ContravariantIndices, int CovariantIndices, typename QuantitiesT_, typename CoordinateSystemT_, typename MapT_>
auto mapRelative(const Tensor<ContravariantIndices, CovariantIndices, QuantitiesT_, CoordinateSystemT_, Absolute>& object,
                 const MapT_& map) {
  return makeAbsolute(map(makeRelative(object)));
}

}  // namespace internal
}  // namespace wuzp
