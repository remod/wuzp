#pragma once

#include <type_traits>

#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveTransformation.hpp"
#include "wuzp/trafo/Transformation.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

/**
 * A passive transformation, which expresses the objects of the coordinate system From in the coordinate system To (see
 * Transformation). The objects it transforms must be expressed in From.
 * PassiveTransformation<From = B, To = A> is X_AB in the monogram notation of Drake
 * (https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html), and ^A_B T in the notation of Craig (Introduction to
 * Robotics, chapter 2): it maps the coordinates of a point in B to its coordinates in A.
 */
template <typename DerivedT_, AnyCoordinateSystem FromT_, AnyCoordinateSystem ToT_>
class PassiveTransformation : public Transformation {
 public:
  using From = FromT_;
  using To = ToT_;

  template <typename Object>
  auto transform(const Object& object) const {
    // Checked before the coordinate system: passive transformations have none, which would fail with a message about a trait.
    static_assert(!std::is_base_of_v<Transformation, Object>,
                  "A passive transformation transforms tensors, not transformations. Compose transformations with operator*.");
    static_assert(std::is_same<typename GetCoordinateSystem<Object>::type, From>::value, "Coordinate systems do not match.");
    return TransformationTraits<DerivedT_, Object>::transform(derived(), object);
  }

 private:
  const DerivedT_& derived() const { return static_cast<const DerivedT_&>(*this); }  // TODO(remod): Use CRTP class template from web.
};

/**
 * The composition of two passive transformations, in the order of the monogram notation of Drake
 * (https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html): X_AB * X_BC = X_AC, i.e. the transformation from C to
 * B, followed by the one from B to A, like ^A_C T = ^A_B T ^B_C T in the notation of Craig. The coordinate systems that meet,
 * From of lhs and To of rhs, must be the same, which wave_geometry (https://github.com/wavelab/wave_geometry) also checks at
 * compile time. See CompositionTraits.
 */
template <typename LhsT_, typename FromLhsT_, typename ToLhsT_, typename RhsT_, typename FromRhsT_, typename ToRhsT_>
auto operator*(const PassiveTransformation<LhsT_, FromLhsT_, ToLhsT_>& lhs, const PassiveTransformation<RhsT_, FromRhsT_, ToRhsT_>& rhs) {
  static_assert(std::is_same_v<FromLhsT_, ToRhsT_>, "Adjacent frames do not match.");
  return CompositionTraits<LhsT_, RhsT_>::compose(static_cast<const LhsT_&>(lhs), static_cast<const RhsT_&>(rhs));
}

// An active and a passive transformation cannot be composed: CompositionTraits rejects them with a message.
template <typename LhsT_, typename RhsT_, typename FromRhsT_, typename ToRhsT_>
auto operator*(const ActiveTransformation<LhsT_>& lhs, const PassiveTransformation<RhsT_, FromRhsT_, ToRhsT_>& rhs) {
  return CompositionTraits<LhsT_, RhsT_>::compose(static_cast<const LhsT_&>(lhs), static_cast<const RhsT_&>(rhs));
}
template <typename LhsT_, typename FromLhsT_, typename ToLhsT_, typename RhsT_>
auto operator*(const PassiveTransformation<LhsT_, FromLhsT_, ToLhsT_>& lhs, const ActiveTransformation<RhsT_>& rhs) {
  return CompositionTraits<LhsT_, RhsT_>::compose(static_cast<const LhsT_&>(lhs), static_cast<const RhsT_&>(rhs));
}

namespace internal {

//! Checks that two passive transformations transform between the same coordinate systems, e.g. to compare them:
//! `static_assert(SameFrames<LhsT_, RhsT_>::value)`. The value is always true, so that a mismatch fails with the message of the
//! static_assert inside.
template <typename LhsT_, typename RhsT_>
struct SameFrames {
  static_assert(std::is_same_v<typename LhsT_::From, typename RhsT_::From>, "'From' coordinate systems do not match.");
  static_assert(std::is_same_v<typename LhsT_::To, typename RhsT_::To>, "'To' coordinate systems do not match.");
  static constexpr bool value = true;
};

//! The passive transformation of the same kind and with the same values and units as PassiveT_, from NewFromT_ to NewToT_,
//! e.g. for an inverse or a composition. Specialized in the headers of the passive transformations.
template <typename PassiveT_, typename NewFromT_, typename NewToT_>
struct WithFrames;

}  // namespace internal

}  // namespace wuzp
