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
template <AnyCoordinateSystem CoordinateSystem, AnyCoordinateSystem From, AnyCoordinateSystem To, typename Rotation_, typename Translation_>
class PassiveProperRigidTransformationTemplate
    : public PassiveTransformation<PassiveProperRigidTransformationTemplate<CoordinateSystem, From, To, Rotation_, Translation_>, From,
                                   To> {
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

template <template <typename, int, int> typename ValuesT_, int Dimension, typename CoordinateSystem, typename From, typename To,
          typename Value>
using PassiveProperRigidTransformation = PassiveProperRigidTransformationTemplate<
    CoordinateSystem, From, To,
    PassiveSpatialRotation<ValuesT_, Value, CoordinateSystem, From, internal::Intermediate<From, To>, Dimension>,
    PassiveSpatialTranslation<ValuesT_, Dimension, Value, To, internal::Intermediate<From, To>, To>>;
template <template <typename, int, int> typename ValuesT_, typename CoordinateSystem, typename From, typename To, typename Value>
using PassiveProperRigidTransformation2 = PassiveProperRigidTransformation<ValuesT_, 2, CoordinateSystem, From, To, Value>;
template <template <typename, int, int> typename ValuesT_, typename CoordinateSystem, typename From, typename To, typename Value>
using PassiveProperRigidTransformation3 = PassiveProperRigidTransformation<ValuesT_, 3, CoordinateSystem, From, To, Value>;

template <typename CoordinateSystem, typename From, typename To, typename Rotation, typename Translation, typename Object>
struct TransformationTraits<PassiveProperRigidTransformationTemplate<CoordinateSystem, From, To, Rotation, Translation>, Object> {
  static auto transform(const PassiveProperRigidTransformationTemplate<CoordinateSystem, From, To, Rotation, Translation>& transformation,
                        const Object& object) {
    return transformation.getTranslation().transform(transformation.getRotation().transform(object));
  }
};

}  // namespace wuzp
