#pragma once

#include <type_traits>
#include <utility>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {
namespace internal {

//! A tensor with the indices, the coordinate system and the absoluteness of `object`, and the given quantities: the result of a
//! linear transformation, whose matrix is expressed in CoordinateSystemT_, e.g. A v for a vector v. The linear transformations
//! multiply their matrix with the quantities of the tensor: their matrix maps one coordinate system into another, possibly of
//! another dimension, so it is not a tensor in one coordinate system.
template <typename CoordinateSystemT_, typename IndicesT_, typename QuantitiesT_, typename ObjectCoordinateSystemT_, typename AbsoluteT_,
          typename QuantitiesOtherT_>
auto withQuantities(const Tensor<IndicesT_, QuantitiesT_, ObjectCoordinateSystemT_, AbsoluteT_>& /*object*/, QuantitiesOtherT_ quantities) {
  static_assert(std::is_same_v<CoordinateSystemT_, ObjectCoordinateSystemT_>, "Coordinate systems do not match.");
  return Tensor<IndicesT_, typename Evaluate<QuantitiesOtherT_>::type, ObjectCoordinateSystemT_, AbsoluteT_>{std::move(quantities)};
}

}  // namespace internal
}  // namespace wuzp
