#pragma once

#include <type_traits>
#include <utility>

#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveTransformation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"
#include "wuzp/trafo/internal/intermediate_coordinate_system.hpp"
#include "wuzp/trafo/internal/rigid.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <AnyCoordinateSystem FromT_, AnyCoordinateSystem ToT_, AnyRotation RotationT_, typename TranslationT_>
class PassiveProperRigidTransformationTemplate;

namespace internal {

//! The proper rigid transformation from NewFromT_ to NewToT_ whose rotation and translation are of the kinds of RotationT_ and
//! TranslationT_, e.g. the result of a composition: the rotation transforms from NewFromT_ to the intermediate coordinate system,
//! the translation from there to NewToT_.
template <typename RotationT_, typename TranslationT_, typename NewFromT_, typename NewToT_>
using PassiveRigidOf =
    PassiveProperRigidTransformationTemplate<NewFromT_, NewToT_,
                                             typename WithFrames<RotationT_, NewFromT_, Intermediate<NewFromT_, NewToT_>>::type,
                                             typename WithFrames<TranslationT_, Intermediate<NewFromT_, NewToT_>, NewToT_>::type>;

//! The proper rigid transformation from NewFromT_ to NewToT_ whose rotation and translation are of the kinds of those of RigidT_,
//! e.g. the result of a composition.
template <typename RigidT_, typename NewFromT_, typename NewToT_>
using PassiveRigidWithFrames = PassiveRigidOf<typename RigidT_::Rotation, typename RigidT_::Translation, NewFromT_, NewToT_>;

}  // namespace internal

/**
 * A passive proper rigid transformation from From to To, which first rotates, then translates: p -> R p + t. The rotation
 * transforms from From to an intermediate coordinate system with the origin of From and the axes of To, the translation from
 * there to To, so the translation t is expressed in To. This is Drake's RigidTransform X_AB = (R_AB, p_AoBo_A) for From = B and
 * To = A (https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rigid_transform.html), whose coordinate transform of spatial
 * vectors is Featherstone's ^A X_B (http://royfeatherstone.org/spatial/v2/xforms.html). The rotation R is a rotation, e.g. a
 * PassiveSpatialRotation, which checks that it is orthogonal with determinant 1 (see PassiveSpatialRotation::isValid()): the
 * inverse, (R^T, -R^T t), relies on it. PassiveProperRigidTransformation is the transformation of a rotation and a spatial
 * translation. Its compositions with rotations are defined in PassiveSpatialRotation.hpp.
 * The translation leaves vectors, covectors and tensors with two upper indices unchanged, so they are only rotated, e.g. a
 * covector w into w R^T and a covariance S into R S R^T, and it rejects the other tensors, e.g. an inertia, which may depend on
 * the origin (see PassiveTranslation).
 * TODO(remod): Introduce class to arbitrarily chain transformations, and make this a typedef.
 */
template <AnyCoordinateSystem FromT_, AnyCoordinateSystem ToT_, AnyRotation RotationT_, typename TranslationT_>
class PassiveProperRigidTransformationTemplate
    : public PassiveTransformation<PassiveProperRigidTransformationTemplate<FromT_, ToT_, RotationT_, TranslationT_>, FromT_, ToT_> {
 public:
  using Rotation = RotationT_;
  using Translation = TranslationT_;
  using ValueT = typename Rotation::ValueT;

  PassiveProperRigidTransformationTemplate() = default;
  PassiveProperRigidTransformationTemplate(Rotation rotation, Translation translation)
      : rotation_{std::move(rotation)}, translation_{std::move(translation)} {}

  void setRotation(const Rotation& rotation) { rotation_ = rotation; }
  const Rotation& getRotation() const { return rotation_; }

  void setTranslation(const Translation& translation) { translation_ = translation; }
  const Translation& getTranslation() const { return translation_; }

  /**
   * The inverse transformation, from To to From, (R^T, -R^T t), which only needs the transpose of the rotation (see
   * internal::invertRigid()). Its translation -R^T t is the vector from the origin of From to the origin of To, expressed in
   * From, the To of the inverse: not -t, unless R t = t.
   */
  auto inverse() const {
    return internal::invertRigid<internal::PassiveRigidWithFrames<PassiveProperRigidTransformationTemplate, ToT_, FromT_>>(*this);
  }

  //! Whether both transformations, which must transform between the same coordinate systems, are approximately equal: their
  //! rotations and their translations are compared with their isApprox().
  template <typename FromOtherT_, typename ToOtherT_, typename RotationOtherT_, typename TranslationOtherT_>
  bool isApprox(const PassiveProperRigidTransformationTemplate<FromOtherT_, ToOtherT_, RotationOtherT_, TranslationOtherT_>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    static_assert(internal::SameFrames<PassiveProperRigidTransformationTemplate, std::remove_cvref_t<decltype(other)>>::value);
    return rotation_.isApprox(other.getRotation(), precision) && translation_.isApprox(other.getTranslation(), precision);
  }

 private:
  Rotation rotation_;
  Translation translation_;
};

template <typename FromT_, typename ToT_, typename RotationT_, typename TranslationT_, typename Object>
struct TransformationTraits<PassiveProperRigidTransformationTemplate<FromT_, ToT_, RotationT_, TranslationT_>, Object> {
  static auto transform(const PassiveProperRigidTransformationTemplate<FromT_, ToT_, RotationT_, TranslationT_>& transformation,
                        const Object& object) {
    return transformation.getTranslation().transform(transformation.getRotation().transform(object));
  }
};

// The compositions X_AB * X_BC = X_AC of proper rigid transformations with each other and with translations are proper rigid
// transformations with a rotation and a translation of the kinds of those of the proper rigid operand (see internal/rigid.hpp).

template <typename FromLhsT_, typename ToLhsT_, typename RotationLhsT_, typename TranslationLhsT_, typename FromRhsT_, typename ToRhsT_,
          typename RotationRhsT_, typename TranslationRhsT_>
struct CompositionTraits<PassiveProperRigidTransformationTemplate<FromLhsT_, ToLhsT_, RotationLhsT_, TranslationLhsT_>,
                         PassiveProperRigidTransformationTemplate<FromRhsT_, ToRhsT_, RotationRhsT_, TranslationRhsT_>> {
  using LhsT = PassiveProperRigidTransformationTemplate<FromLhsT_, ToLhsT_, RotationLhsT_, TranslationLhsT_>;
  static auto compose(const LhsT& lhs,
                      const PassiveProperRigidTransformationTemplate<FromRhsT_, ToRhsT_, RotationRhsT_, TranslationRhsT_>& rhs) {
    return internal::composeRigid<internal::PassiveRigidWithFrames<LhsT, FromRhsT_, ToLhsT_>>(lhs, rhs);
  }
};

template <typename FromLhsT_, typename ToLhsT_, typename RotationLhsT_, typename TranslationLhsT_,
          template <typename, int, int> typename ValuesRhsT_, int DimensionRhsV_, typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_,
          typename... UnitsRhsT_>
struct CompositionTraits<PassiveProperRigidTransformationTemplate<FromLhsT_, ToLhsT_, RotationLhsT_, TranslationLhsT_>,
                         PassiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, FromRhsT_, ToRhsT_, UnitsRhsT_...>> {
  using LhsT = PassiveProperRigidTransformationTemplate<FromLhsT_, ToLhsT_, RotationLhsT_, TranslationLhsT_>;
  static auto compose(const LhsT& lhs,
                      const PassiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, FromRhsT_, ToRhsT_, UnitsRhsT_...>& rhs) {
    return internal::composeRigidAndTranslation<internal::PassiveRigidWithFrames<LhsT, FromRhsT_, ToLhsT_>>(lhs, rhs);
  }
};

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_,
          typename... UnitsLhsT_, typename FromRhsT_, typename ToRhsT_, typename RotationRhsT_, typename TranslationRhsT_>
struct CompositionTraits<PassiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, FromLhsT_, ToLhsT_, UnitsLhsT_...>,
                         PassiveProperRigidTransformationTemplate<FromRhsT_, ToRhsT_, RotationRhsT_, TranslationRhsT_>> {
  using RhsT = PassiveProperRigidTransformationTemplate<FromRhsT_, ToRhsT_, RotationRhsT_, TranslationRhsT_>;
  static auto compose(const PassiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, FromLhsT_, ToLhsT_, UnitsLhsT_...>& lhs,
                      const RhsT& rhs) {
    return internal::composeTranslationAndRigid<internal::PassiveRigidWithFrames<RhsT, FromRhsT_, ToLhsT_>>(lhs, rhs);
  }
};

}  // namespace wuzp
