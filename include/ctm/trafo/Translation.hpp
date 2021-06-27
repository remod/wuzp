#pragma once

#include "ctm/trafo/Transformation.hpp"

namespace ctm {

template <typename Derived, typename From, typename To>
class TranslationBase : public PassiveTransformation<Derived, From, To> {};

template <int Dimension, typename Scalar, typename From, typename To>
class Translation : public TranslationBase<Translation<Dimension, Scalar, From, To>, From, To> {
  friend TransformationTraits<Translation<Dimension, Scalar, From, To>, Position<Dimension, Scalar, From>>;

 public:
  using Impl = Vector<Dimension, Scalar, From>;  // TODO(remod): Use eigen directly? Interface should be consistent with Quaternion

  explicit Translation(Impl impl) : impl_(impl) {}

 private:
  Impl impl_;
};

template <int Dimension, typename From, typename To, typename Scalar>
struct TransformationTraits<Translation<Dimension, Scalar, From, To>, Vector<Dimension, Scalar, From>> {
static auto transform(const Translation<Dimension, Scalar, From, To>& /*transformation*/, const Vector<Dimension, Scalar, From>& object) {
  return Vector<Dimension, Scalar, To>(object.getValues());
}
};

template <int Dimension, typename From, typename To, typename Scalar>
struct TransformationTraits<Translation<Dimension, Scalar, From, To>, Position<Dimension, Scalar, From>> {
static auto transform(const Translation<Dimension, Scalar, From, To>& transformation, const Position<Dimension, Scalar, From>& object) {
  return Position<Dimension, Scalar, To>((transformation.impl_ + object).getValues());
}
};

template <typename Scalar, typename From, typename To>
using Translation2D = Translation<2, Scalar, From, To>;

template <typename Scalar, typename From, typename To>
using Translation3D = Translation<3, Scalar, From, To>;

}  // namespace ctm
