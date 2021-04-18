#pragma once

#include "ctm/Frame.hpp"

namespace ctm {

template <typename Lhs, typename Rhs>
struct TransformationTraits {
  static_assert(FakeDependency<TransformationTraits<Lhs, Rhs>>::value, "'TransformationTraits' trait not specialized.");
};

class Transformation {};

// Translation, Rotation, Reflection, Dilation.

class ActiveTransformation : public Transformation {};

template <typename From, typename To>
class PassiveTransformation : public Transformation {};

// First rotate, then translate.
template <typename From, typename To, typename Rotation, typename Translation>
class ProperRigidTransformationTemplate : public PassiveTransformation<From, To> {
 public:
  ProperRigidTransformationTemplate() = default;
  ProperRigidTransformationTemplate(Rotation rotation, Translation translation)
      : rotation_(std::move(rotation)), translation_(std::move(translation)) {}

  void setRotation(const Rotation& rotation) { rotation_ = rotation; }
  const Rotation& getRotation() const { return rotation_; }

  void setTranslation(const Translation& translation) { translation_ = translation; }
  const Translation& getTranslation() const { return translation_; }

  template <typename Object>
  auto transform(const Object& object) const {
    static_assert(!std::is_same<typename GetFrame<Object>::type, From>::value, "Frame mismatch.");
    return TransformationTraits<This, Object>::transform(*this, object);
  }

 private:
  using This = ProperRigidTransformationTemplate<From, To, Rotation, Translation>;

  Rotation rotation_;
  Translation translation_;
};

}  // namespace ctm
