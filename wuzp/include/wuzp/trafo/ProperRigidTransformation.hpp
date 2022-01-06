#pragma once

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/trafo/Rotation.hpp"
#include "wuzp/trafo/Transformation.hpp"
#include "wuzp/trafo/Translation.hpp"

namespace wuzp {

// First rotate, then translate.
// TODO(remod): Introduce class to arbitrarily chain transformations, and make this a typedef.
template <typename CoordinateSystem, typename From, typename To, typename Rotation_, typename Translation_>
class ProperRigidTransformationTemplate
    : public PassiveTransformation<ProperRigidTransformationTemplate<CoordinateSystem, From, To, Rotation_, Translation_>, From, To> {
 public:
  using Rotation = Rotation_;
  using Translation = Translation_;

  ProperRigidTransformationTemplate() = default;
  ProperRigidTransformationTemplate(Rotation rotation, Translation translation)
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
using ProperRigidTransformation = ProperRigidTransformationTemplate<
    CoordinateSystem, From, To, Rotation<ValueMatrix, Scalar, CoordinateSystem, From, internal::Intermediate<From, To>, Dimension>,
    SpatialTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem, internal::Intermediate<From, To>, To>>;
template <template <typename, int, int> typename ValueMatrix, typename CoordinateSystem, typename From, typename To, typename Scalar>
using ProperRigidTransformation2D = ProperRigidTransformation<ValueMatrix, 2, CoordinateSystem, From, To, Scalar>;
template <template <typename, int, int> typename ValueMatrix, typename CoordinateSystem, typename From, typename To, typename Scalar>
using ProperRigidTransformation3D = ProperRigidTransformation<ValueMatrix, 3, CoordinateSystem, From, To, Scalar>;

template <typename CoordinateSystem, typename From, typename To, typename Rotation, typename Translation, typename Object>
struct TransformationTraits<ProperRigidTransformationTemplate<CoordinateSystem, From, To, Rotation, Translation>, Object> {
  static auto transform(const ProperRigidTransformationTemplate<CoordinateSystem, From, To, Rotation, Translation>& transformation,
                        const Object& object) {
    const auto translationCoordinateFrame = transformation.getTranslation();
    const auto translationIntermediate =
        typename CastCoordinateSystem<std::decay_t<decltype(translationCoordinateFrame)>, internal::Intermediate<From, To>>::type(
            translationCoordinateFrame.getTensor().getCoordinateMatrix().getQuantityMatrix());
    return translationIntermediate.transform(transformation.getRotation().transform(object));
  }
};

}  // namespace wuzp
