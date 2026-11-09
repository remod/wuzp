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
 * It is specialized where the result is again a transformation of wuzp: linear transformations with linear transformations,
 * translations with translations, and proper rigid transformations with proper rigid transformations and with translations.
 * Every other combination is rejected, e.g. a translation with a rotation, which are not told apart from other linear
 * transformations yet, or an active with a passive transformation.
 */
template <typename LhsT_, typename RhsT_>
struct CompositionTraits {
  static_assert(FailOnInstantiation<CompositionTraits<LhsT_, RhsT_>>::value, "These transformations cannot be composed.");
};

}  // namespace wuzp
