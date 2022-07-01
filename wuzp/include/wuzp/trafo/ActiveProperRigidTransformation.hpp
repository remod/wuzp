#pragma once

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/trafo/ActiveRotation.hpp"
#include "wuzp/trafo/ActiveTransformation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

// First rotate, then translate.
// TODO(remod): Introduce class to arbitrarily chain transformations, and make this a typedef.
template <typename CoordinateSystem, typename Rotation_, typename Translation_>
class ActiveProperRigidTransformationTemplate
    : public ActiveTransformation<ActiveProperRigidTransformationTemplate<CoordinateSystem, Rotation_, Translation_>> {
 public:
  using Rotation = Rotation_;
  using Translation = Translation_;

  ActiveProperRigidTransformationTemplate() = default;
  ActiveProperRigidTransformationTemplate(Rotation rotation, Translation translation)
      : rotation_(std::move(rotation)), translation_(std::move(translation)) {}

  void setRotation(const Rotation& rotation) { rotation_ = rotation; }
  const Rotation& getRotation() const { return rotation_; }

  void setTranslation(const Translation& translation) { translation_ = translation; }
  const Translation& getTranslation() const { return translation_; }

 private:
  Rotation rotation_;
  Translation translation_;
};

template <template <typename, int, int> typename ValuesT_, int Dimension, typename CoordinateSystem, typename Value>
using ActiveProperRigidTransformation =
    ActiveProperRigidTransformationTemplate<CoordinateSystem, ActiveSpatialRotation<ValuesT_, Value, CoordinateSystem, Dimension>,
                                            ActiveSpatialTranslation<ValuesT_, Dimension, Value, CoordinateSystem>>;
template <template <typename, int, int> typename ValuesT_, typename CoordinateSystem, typename Value>
using ActiveProperRigidTransformation2 = ActiveProperRigidTransformation<ValuesT_, 2, CoordinateSystem, Value>;
template <template <typename, int, int> typename ValuesT_, typename CoordinateSystem, typename Value>
using ActiveProperRigidTransformation3 = ActiveProperRigidTransformation<ValuesT_, 3, CoordinateSystem, Value>;

template <typename CoordinateSystem, typename Rotation, typename Translation, typename Object>
struct TransformationTraits<ActiveProperRigidTransformationTemplate<CoordinateSystem, Rotation, Translation>, Object> {
  static auto transform(const ActiveProperRigidTransformationTemplate<CoordinateSystem, Rotation, Translation>& transformation,
                        const Object& object) {
    return transformation.getTranslation().transform(transformation.getRotation().transform(object));
  }
};

}  // namespace wuzp
