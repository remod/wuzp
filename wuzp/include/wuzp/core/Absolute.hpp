#pragma once

#include "wuzp/core/internal/AbsoluteImpl.hpp"
#include "wuzp/core/traits.hpp"

// Absolute objects are points, relative objects are vectors. Their operations follow the rules of affine spaces, like CGAL's
// points and vectors (https://doc.cgal.org/latest/Kernel_23/index.html) and mp-units' quantity points
// (https://mpusz.github.io/mp-units/latest/users_guide/framework_basics/the_affine_space/): p - q is a vector, p + v, v + p and
// p - v are points. Every other operation on a point depends on the origin of its coordinate system, e.g. the product with a
// scalar, the negation or the inverse, so it is rejected: in the terms of Atkey, Johann and Kennedy, "Abstraction and invariance
// for algebraically indexed types" (POPL 2013, https://doi.org/10.1145/2429069.2429082), it is not invariant under translations.
// Affine combinations are written p + (q - p) * t. The traits of this file check the absoluteness of the operands and compute
// that of the result, like the traits of Indices.hpp do for indices; Tensor.hpp combines them.

namespace wuzp {

using Absolute = internal::AbsoluteImpl<true>;
using Relative = internal::AbsoluteImpl<false>;

template <bool ValueLhs_, bool ValueRhs_>
struct Sum<internal::AbsoluteImpl<ValueLhs_>, internal::AbsoluteImpl<ValueRhs_>> {
  static_assert(!ValueLhs_ || !ValueRhs_, "Two absolute objects cannot be summed.");
  using type = typename IfThenElse<ValueLhs_ || ValueRhs_, Absolute, Relative>::type;
};

template <bool ValueLhs_, bool ValueRhs_>
struct Difference<internal::AbsoluteImpl<ValueLhs_>, internal::AbsoluteImpl<ValueRhs_>> {
  static_assert(ValueLhs_ || !ValueRhs_, "An absolute object cannot be subtracted from a relative object.");
  using type = typename IfThenElse<ValueLhs_, typename IfThenElse<ValueRhs_, Relative, Absolute>::type, Relative>::type;
};

template <bool Value_>
struct Negation<internal::AbsoluteImpl<Value_>> {
  static_assert(!Value_, "An absolute object cannot be negated.");
  using type = Relative;
};

//! Products, e.g. with a scalar, and the dot, inner, outer and cross products. Scalars are relative.
template <bool ValueLhs_, bool ValueRhs_>
struct Product<internal::AbsoluteImpl<ValueLhs_>, internal::AbsoluteImpl<ValueRhs_>> {
  static_assert(!ValueLhs_ && !ValueRhs_, "An absolute object cannot appear in a multiplication.");
  using type = Relative;
};

//! Quotients, e.g. by a scalar. Scalars are relative.
template <bool ValueLhs_, bool ValueRhs_>
struct Division<internal::AbsoluteImpl<ValueLhs_>, internal::AbsoluteImpl<ValueRhs_>> {
  static_assert(!ValueLhs_ && !ValueRhs_, "An absolute object cannot appear in a division.");
  using type = Relative;
};

template <bool Value_, typename ExponentT_>
struct Power<internal::AbsoluteImpl<Value_>, ExponentT_> {
  static_assert(!Value_, "An absolute object cannot be powered.");
  using type = Relative;
};

//! The matrix inverse and the elementwise reciprocal.
template <bool Value_>
struct Inverse<internal::AbsoluteImpl<Value_>> {
  static_assert(!Value_, "An absolute object cannot be inverted.");
  using type = Relative;
};

//! The norm and the squared norm. The norm of a point is its distance from the origin, so it depends on the origin; the distance
//! of two points is the norm of their difference.
template <bool Value_>
struct Norm<internal::AbsoluteImpl<Value_>> {
  static_assert(!Value_, "The norm of a point depends on the origin. Use the norm of a difference of points, e.g. (p - q).norm().");
  using type = Relative;
};

template <bool Value_>
struct IsAbsolute<internal::AbsoluteImpl<Value_>> {
  static constexpr bool value = Value_;
};

//! The vector from the origin to a point (see makeRelative()).
template <bool Value_>
struct MakeRelative<internal::AbsoluteImpl<Value_>> {
  static_assert(Value_, "The object is already relative.");
  using type = Relative;
};

//! The point at a vector from the origin (see makeAbsolute()).
template <bool Value_>
struct MakeAbsolute<internal::AbsoluteImpl<Value_>> {
  static_assert(!Value_, "The object is already absolute.");
  using type = Absolute;
};

}  // namespace wuzp
