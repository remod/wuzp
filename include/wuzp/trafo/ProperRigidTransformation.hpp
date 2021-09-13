#pragma once

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/trafo/Rotation.hpp"
#include "wuzp/trafo/Transformation.hpp"
#include "wuzp/trafo/Translation.hpp"

namespace wuzp {

// First rotate, then translate.
// TODO(remod): Introduce class to arbitrarily chain transformations, and make this a typedef.
template <typename From, typename To, typename Rotation, typename Translation>
class ProperRigidTransformationTemplate
    : public PassiveTransformation<ProperRigidTransformationTemplate<From, To, Rotation, Translation>, From, To> {
 public:
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

template <int Dimension, typename From, typename Intermediate, typename To, typename Rotation, typename TranslationScalar>
using ProperRigidTransformation =
    ProperRigidTransformationTemplate<From, Intermediate, Rotation, Translation<Dimension, TranslationScalar, Intermediate, To>>;

template <typename From, typename Intermediate, typename To, typename Scalar>
using ProperRigidTransformation2D = ProperRigidTransformation<2, From, Intermediate, To, Scalar, Scalar>;

template <typename From, typename Intermediate, typename To, typename Scalar>
using ProperRigidTransformation3D = ProperRigidTransformation<3, From, Intermediate, To, Quaternion<Scalar, From, Intermediate>, Scalar>;

template <typename From, typename Intermediate, typename To, typename Scalar, typename Object>
struct TransformationTraits<ProperRigidTransformation3D<From, Intermediate, To, Scalar>, Object> {
  static auto transform(const ProperRigidTransformation3D<From, Intermediate, To, Scalar>& transformation, const Object& object) {
    return transformation.getTranslation().transform(transformation.getRotation().transform(object));
  }
};

}  // namespace wuzp
