#pragma once

#include <utility>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveTransformation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"
#include "wuzp/trafo/internal/intermediate_coordinate_system.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

// First rotate, then translate. The rotation transforms from From to an intermediate coordinate system with the origin of From and
// the axes of To, the translation from there to To. The translation is therefore expressed in To.
// TODO(remod): Introduce class to arbitrarily chain transformations, and make this a typedef.
template <AnyCoordinateSystem CoordinateSystemT_, AnyCoordinateSystem FromT_, AnyCoordinateSystem ToT_, typename Rotation_,
          typename Translation_>
class PassiveProperRigidTransformationTemplate
    : public PassiveTransformation<PassiveProperRigidTransformationTemplate<CoordinateSystemT_, FromT_, ToT_, Rotation_, Translation_>,
                                   FromT_, ToT_> {
 public:
  using Rotation = Rotation_;
  using Translation = Translation_;

  PassiveProperRigidTransformationTemplate() = default;
  PassiveProperRigidTransformationTemplate(Rotation rotation, Translation translation)
      : rotation_{std::move(rotation)}, translation_{std::move(translation)} {}

  void setRotation(const Rotation& rotation) { rotation_ = rotation; }
  const Rotation& getRotation() const { return rotation_; }

  void setTranslation(const Translation& translation) { translation_ = translation; }
  const Translation& getTranslation() const { return translation_; }

 private:
  Rotation rotation_;
  Translation translation_;
};

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          typename ValueT_>
using PassiveProperRigidTransformation = PassiveProperRigidTransformationTemplate<
    CoordinateSystemT_, FromT_, ToT_,
    PassiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, internal::Intermediate<FromT_, ToT_>, DimensionV_>,
    PassiveSpatialTranslation<ValuesT_, DimensionV_, ValueT_, ToT_, internal::Intermediate<FromT_, ToT_>, ToT_>>;
template <template <typename, int, int> typename ValuesT_, typename CoordinateSystemT_, typename FromT_, typename ToT_, typename ValueT_>
using PassiveProperRigidTransformation2 = PassiveProperRigidTransformation<ValuesT_, 2, CoordinateSystemT_, FromT_, ToT_, ValueT_>;
template <template <typename, int, int> typename ValuesT_, typename CoordinateSystemT_, typename FromT_, typename ToT_, typename ValueT_>
using PassiveProperRigidTransformation3 = PassiveProperRigidTransformation<ValuesT_, 3, CoordinateSystemT_, FromT_, ToT_, ValueT_>;

template <typename CoordinateSystemT_, typename FromT_, typename ToT_, typename RotationT_, typename TranslationT_, typename Object>
struct TransformationTraits<PassiveProperRigidTransformationTemplate<CoordinateSystemT_, FromT_, ToT_, RotationT_, TranslationT_>, Object> {
  static auto transform(
      const PassiveProperRigidTransformationTemplate<CoordinateSystemT_, FromT_, ToT_, RotationT_, TranslationT_>& transformation,
      const Object& object) {
    return transformation.getTranslation().transform(transformation.getRotation().transform(object));
  }
};

}  // namespace wuzp
