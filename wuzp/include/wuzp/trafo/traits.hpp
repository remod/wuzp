#pragma once

#include <type_traits>

#include "wuzp/core/helpers.hpp"
#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

/**
 * How a transformation transforms an object: `static auto transform(const TransformationT_&, const Object&)` returns the result.
 * It is specialized for the tensors that a transformation transforms, and for the conjugations `a b a^-1` that active
 * transformations define (see "Conventions" in DESIGN.md). Any other transformation as the object is rejected with a message
 * that says to compose transformations with operator* (see CompositionTraits).
 */
template <typename TransformationT_, typename Object>
struct TransformationTraits {
  static_assert(std::is_base_of_v<Transformation, Object> || FailOnInstantiation<TransformationTraits<TransformationT_, Object>>::value,
                "'TransformationTraits' trait not specialized.");
  static_assert(!std::is_base_of_v<Transformation, Object>,
                "This transformation does not transform this transformation. Compose transformations with operator*.");
};

/**
 * The composition lhs * rhs of two transformations, which applies rhs first, then lhs: (lhs * rhs).transform(x) is
 * lhs.transform(rhs.transform(x)), like the product of their matrices. `static auto compose(const LhsT_&, const RhsT_&)` returns
 * it. The operators check the coordinate systems first (see ActiveTransformation.hpp and PassiveTransformation.hpp).
 * It is specialized where the result is again a transformation of wuzp: linear transformations, including rotations and
 * Lorentz transformations, with linear transformations (a rotation with a rotation is a rotation, a Lorentz transformation with
 * a Lorentz transformation a Lorentz transformation), translations with translations, and proper rigid transformations,
 * rotations and translations with each other, except a translation with a translation, into proper rigid transformations.
 * Every other combination is rejected, e.g. a translation with a linear transformation that is not a rotation, whose
 * composition is affine but not rigid, or an active with a passive transformation.
 */
template <typename LhsT_, typename RhsT_>
struct CompositionTraits {
  static_assert(FailOnInstantiation<CompositionTraits<LhsT_, RhsT_>>::value, "These transformations cannot be composed.");
};

/**
 * The tolerance of the validity checks of rotations and Lorentz transformations, for values of the type ValueT_: the largest
 * absolute deviation of an element of R R^T from the identity, or of Lambda^T eta Lambda from eta (see
 * ActiveSpatialRotation::isValid() and LorentzTransformation::isValid()). It is separate from Precision, the default of
 * isApprox(), which is too strict for transformations whose rounding errors accumulated, e.g. in compositions: Drake's
 * RotationMatrix accepts 128 machine epsilons (about 3e-14 for double,
 * https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rotation_matrix.html), Sophus 1e-10 (https://github.com/strasdat/Sophus),
 * and wuzp leaves a margin for longer chains: 1e-9 for double and 1e-4 for float. Integer matrices are exact, so their tolerance
 * is 0, e.g. for rotations by multiples of pi/2. Specialize it for other value types.
 */
template <typename ValueT_>
struct ValidityTolerance {
  static_assert(FailOnInstantiation<ValidityTolerance<ValueT_>>::value, "'ValidityTolerance' trait not specialized for given types.");
};

template <>
struct ValidityTolerance<int> {
  static constexpr int value() { return 0; }
};

template <>
struct ValidityTolerance<float> {
  static constexpr float value() { return 1e-4f; }
};

template <>
struct ValidityTolerance<double> {
  static constexpr double value() { return 1e-9; }
};

namespace internal {

//! Whether T is a rotation, i.e. an ActiveSpatialRotation or a PassiveSpatialRotation. Specialized in their headers.
template <typename T>
struct IsRotation : std::false_type {};

}  // namespace internal

//! A rotation, i.e. an ActiveSpatialRotation or a PassiveSpatialRotation, whose matrix is orthogonal with determinant 1, so that
//! its inverse is its transpose: e.g. the rotation of a proper rigid transformation.
template <typename T>
concept AnyRotation = internal::IsRotation<T>::value;

}  // namespace wuzp
