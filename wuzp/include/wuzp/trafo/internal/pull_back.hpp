#pragma once

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/internal/with_quantities.hpp"

namespace wuzp {
namespace internal {

// The pullback of the covariant tensors, expressed in ToT_, into FromT_ by a matrix A that maps the vectors of FromT_ into ToT_,
// which Jacobians and linear transformations share: a Jacobian pulls them back by its matrix (see Jacobian::pullback()), and a
// passive linear transformation, i.e. a change of basis, transforms them from From to To by pulling them back by its inverse,
// which maps the vectors of To into From (see PassiveLinearTransformation). An active linear transformation pulls them back by its
// inverse within its coordinate system, FromT_ = ToT_ (see ActiveLinearTransformation). The pullback needs no inverse of A, so A
// need not be square. The matrix maps one coordinate system into another, so it is multiplied with the quantities of the tensors
// (see withQuantities()).
// These are overloads rather than one function with `if constexpr`, like pushForward().

//! A covector, e.g. a force, is pulled back like w' = w A.
template <typename FromT_, typename ToT_, typename MatrixT_, typename QuantitiesT_, typename CoordinateSystemT_>
auto pullBack(const MatrixT_& matrix, const Tensor<Indices<Down>, QuantitiesT_, CoordinateSystemT_, Relative>& object) {
  return castCoordinateSystem<FromT_>(withQuantities<ToT_>(object, innerProduct(object.getQuantities(), matrix)));
}

//! A tensor with two lower indices, e.g. a mass matrix, is pulled back like M' = A^T M A.
template <typename FromT_, typename ToT_, typename MatrixT_, typename QuantitiesT_, typename CoordinateSystemT_>
auto pullBack(const MatrixT_& matrix, const Tensor<Indices<Down, Down>, QuantitiesT_, CoordinateSystemT_, Relative>& object) {
  return castCoordinateSystem<FromT_>(
      withQuantities<ToT_>(object, innerProduct(matrix.transpose(), innerProduct(object.getQuantities(), matrix))));
}

//! Every other tensor is rejected, which only Jacobian::pullback() passes on: vectors and tensors with two upper indices are
//! pushed forward instead (see pushForward()), and the indices of a mixed tensor would have to be mapped in opposite directions.
template <typename FromT_, typename ToT_, typename MatrixT_, typename ObjectT_>
void pullBack(const MatrixT_& /*matrix*/, const ObjectT_& /*object*/) {
  static_assert(FailOnInstantiation<ObjectT_>::value,
                "The pullback by a Jacobian maps covectors and tensors with two lower indices. Use transform() for vectors and tensors "
                "with two upper indices.");
}

}  // namespace internal
}  // namespace wuzp
