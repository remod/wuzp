#pragma once

namespace ctm {

template <typename Transformation, typename Object>
struct TransformationTraits;

template <typename Translation, typename Rotation, typename From, typename To>
class TransformationBase {
  TransformationBase() = default;
  TransformationBase(Translation translation, Rotation rotation) : translation_(std::move(translation)), rotation_(std::move(rotation)) {}

  void setTranslation(const Translation& translation) { translation_ = translation; }
  const Translation& getTranslation() const { return translation_; }

  void setRotation(const Rotation& rotation) { rotation_ = rotation; }
  const Rotation& getRotation() const { return rotation_; }

  template <typename Object, typename Return>
  Return transform(const Object& object) {
    return TransformationTraits<This, Object>(*this, object);
  }

 private:
  using This = TransformationBase<Translation, Rotation, From, To>;

  Translation translation_;
  Rotation rotation_;
};

// template <typename Scalar, typename From, typename To>
// using TransformationUsingQuaternion = TransformationBase<Eigen::Matrix<Scalar, 3, 1> , Eigen::Quaternion<Scalar>, From, To>;

template <typename Transformation, typename Object>
struct TransformationTraits;

}  // namespace ctm
