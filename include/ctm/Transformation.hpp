#pragma once

namespace ctm {

template <typename Transformation, typename Object>
struct TransformationTraits;

class Transformation {};

// Translation, Rotation, Reflection, Dilation.

class ActiveTransformation : public Transformation {};

template <typename From, typename To>
class PassiveTransformation : public Transformation {};

// First rotate, then translate.
template <typename From, typename To, typename Rotation, typename Translation>
class ProperRigidTransformation : public PassiveTransformation<From, To> {
 public:
  ProperRigidTransformation() = default;
  ProperRigidTransformation(Rotation rotation, Translation translation) : rotation_(std::move(rotation)), translation_(std::move(translation)) {}

  void setRotation(const Rotation& rotation) { rotation_ = rotation; }
  const Rotation& getRotation() const { return rotation_; }

  void setTranslation(const Translation& translation) { translation_ = translation; }
  const Translation& getTranslation() const { return translation_; }

  template <typename Object>
  auto transform(const Object& object) const {
    return TransformationTraits<This, Object>::transform(*this, object);
  }

 private:
  using This = ProperRigidTransformation<From, To, Rotation, Translation>;

  Rotation rotation_;
  Translation translation_;
};

}  // namespace ctm
