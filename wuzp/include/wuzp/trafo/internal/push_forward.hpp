#pragma once

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/internal/with_quantities.hpp"

namespace wuzp {
namespace internal {

// The pushforward of the contravariant tensors, expressed in FromT_, into ToT_ by a matrix A, which linear transformations and
// Jacobians share (see ActiveLinearTransformation, PassiveLinearTransformation and Jacobian), like the pullback of the covariant
// tensors (see pullBack()). An active linear transformation maps its coordinate system into itself, FromT_ = ToT_. The matrix maps
// one coordinate system into another, so it is multiplied with the quantities of the tensors (see withQuantities()).
// These are overloads rather than one function with `if constexpr`, like translate().

//! A vector, or a batch of vectors, transforms like v' = A v. A point is mapped through its coordinates relative to the origin,
//! o + A (p - o), whose components are those of A p: the result is a point again, since withQuantities() keeps the absoluteness
//! of the object, and the components are multiplied as quantities, so the point is not copied into a vector first. Only linear
//! transformations map points: a Jacobian rejects them before.
template <typename FromT_, typename ToT_, typename MatrixT_, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
auto pushForward(const MatrixT_& matrix, const Tensor<Indices<Up>, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
  return withQuantities<FromT_, ToT_>(object, innerProduct(matrix, object.getQuantities()));
}

//! A tensor with two upper indices, e.g. a covariance matrix, transforms like T' = A T A^T.
template <typename FromT_, typename ToT_, typename MatrixT_, typename QuantitiesT_, typename CoordinateSystemT_>
auto pushForward(const MatrixT_& matrix, const Tensor<Indices<Up, Up>, QuantitiesT_, CoordinateSystemT_, Relative>& object) {
  return withQuantities<FromT_, ToT_>(object, innerProduct(innerProduct(matrix, object.getQuantities()), matrix.transpose()));
}

}  // namespace internal
}  // namespace wuzp
