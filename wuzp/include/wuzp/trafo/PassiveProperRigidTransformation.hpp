#pragma once

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/trafo/PassiveRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"
#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

// First rotate, then translate.
// TODO(remod): Introduce class to arbitrarily chain transformations, and make this a typedef.
template <typename CoordinateSystem, typename From, typename To, typename Rotation_, typename Translation_>
class PassiveProperRigidTransformationTemplate
    : public PassiveTransformation<PassiveProperRigidTransformationTemplate<CoordinateSystem, From, To, Rotation_, Translation_>, From,
                                   To> {
 public:
  using Rotation = Rotation_;
  using Translation = Translation_;

  PassiveProperRigidTransformationTemplate() = default;
  PassiveProperRigidTransformationTemplate(Rotation rotation, Translation translation)
      : rotation_(std::move(rotation)), translation_(std::move(translation)) {}

  void setRotation(const Rotation& rotation) { rotation_ = rotation; }
  const Rotation& getRotation() const { return rotation_; }

  void setTranslation(const Translation& translation) { translation_ = translation; }
  const Translation& getTranslation() const { return translation_; }

 private:
  Rotation rotation_;
  Translation translation_;
};

namespace internal {

template <typename From, typename To>
struct IntermediateName;
template <typename From, typename To>
using Intermediate = CoordinateSystem<IntermediateName<From, To>>;

}  // namespace internal

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename CoordinateSystem, typename From, typename To,
          typename Scalar>
using PassiveProperRigidTransformation = PassiveProperRigidTransformationTemplate<
    CoordinateSystem, From, To,
    PassiveSpatialRotation<ValueMatrix, Scalar, CoordinateSystem, From, internal::Intermediate<From, To>, Dimension>,
    PassiveSpatialTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem, internal::Intermediate<From, To>, To>>;
template <template <typename, int, int> typename ValueMatrix, typename CoordinateSystem, typename From, typename To, typename Scalar>
using PassiveProperRigidTransformation2 = PassiveProperRigidTransformation<ValueMatrix, 2, CoordinateSystem, From, To, Scalar>;
template <template <typename, int, int> typename ValueMatrix, typename CoordinateSystem, typename From, typename To, typename Scalar>
using PassiveProperRigidTransformation3 = PassiveProperRigidTransformation<ValueMatrix, 3, CoordinateSystem, From, To, Scalar>;

template <typename CoordinateSystem, typename From, typename To, typename Rotation, typename Translation, typename Object>
struct TransformationTraits<PassiveProperRigidTransformationTemplate<CoordinateSystem, From, To, Rotation, Translation>, Object> {
  static auto transform(const PassiveProperRigidTransformationTemplate<CoordinateSystem, From, To, Rotation, Translation>& transformation,
                        const Object& object) {
    const auto translationCoordinateFrame = transformation.getTranslation();
    const auto translationIntermediate =
        typename CastCoordinateSystem<std::decay_t<decltype(translationCoordinateFrame)>, internal::Intermediate<From, To>>::type(
            translationCoordinateFrame.getTensor().getQuantityMatrix());
    return translationIntermediate.transform(transformation.getRotation().transform(object));
  }
};

}  // namespace wuzp
