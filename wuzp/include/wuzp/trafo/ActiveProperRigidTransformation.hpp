#pragma once

#include <utility>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/ActiveTransformation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

// First rotate, then translate.
// TODO(remod): Introduce class to arbitrarily chain transformations, and make this a typedef.
// TODO(remod): Add test which checks that active and passive transformation cancel each other (to proof order).
template <AnyCoordinateSystem CoordinateSystemT_, typename Rotation_, typename Translation_>
class ActiveProperRigidTransformationTemplate
    : public ActiveTransformation<ActiveProperRigidTransformationTemplate<CoordinateSystemT_, Rotation_, Translation_>> {
 public:
  using Rotation = Rotation_;
  using Translation = Translation_;

  ActiveProperRigidTransformationTemplate() = default;
  ActiveProperRigidTransformationTemplate(Rotation rotation, Translation translation)
      : rotation_{std::move(rotation)}, translation_{std::move(translation)} {}

  void setRotation(const Rotation& rotation) { rotation_ = rotation; }
  const Rotation& getRotation() const { return rotation_; }

  void setTranslation(const Translation& translation) { translation_ = translation; }
  const Translation& getTranslation() const { return translation_; }

 private:
  Rotation rotation_;
  Translation translation_;
};

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename CoordinateSystemT_, typename ValueT_>
using ActiveProperRigidTransformation =
    ActiveProperRigidTransformationTemplate<CoordinateSystemT_, ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>,
                                            ActiveSpatialTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_>>;
template <template <typename, int, int> typename ValuesT_, typename CoordinateSystemT_, typename ValueT_>
using ActiveProperRigidTransformation2 = ActiveProperRigidTransformation<ValuesT_, 2, CoordinateSystemT_, ValueT_>;
template <template <typename, int, int> typename ValuesT_, typename CoordinateSystemT_, typename ValueT_>
using ActiveProperRigidTransformation3 = ActiveProperRigidTransformation<ValuesT_, 3, CoordinateSystemT_, ValueT_>;

template <typename CoordinateSystemT_, typename RotationT_, typename TranslationT_, typename Object>
struct TransformationTraits<ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>, Object> {
  static auto transform(const ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>& transformation,
                        const Object& object) {
    return transformation.getTranslation().transform(transformation.getRotation().transform(object));
  }
};

}  // namespace wuzp
