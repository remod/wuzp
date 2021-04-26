#pragma once

#include "ctm/Frame.hpp"

namespace ctm {

template <typename Transformation, typename Object>
struct TransformationTraits {
  static_assert(FakeDependency<TransformationTraits<Transformation, Object>>::value, "'TransformationTraits' trait not specialized.");
};

class Transformation {};

// Translation, Rotation, Reflection, Dilation.

class ActiveTransformation : public Transformation {};

template <typename Derived, typename From, typename To>
class PassiveTransformation : public Transformation {
 public:
  template <typename Object>
  auto transform(const Object& object) const {
    static_assert(!std::is_same<typename GetFrame<Object>::type, From>::value, "Frame mismatch.");
    return TransformationTraits<Derived, Object>::transform(derived(), object);
  }

 private:
  const Derived& derived() const { return static_cast<const Derived&>(*this); }  // TODO(remod): Use CRTP class template from web.
};

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

}  // namespace ctm
