#pragma once

#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

template <typename Derived, typename From, typename To>
class TranslationBase : public PassiveTransformation<Derived, From, To> {};

template <int Dimension, typename Scalar, typename From, typename To>
class Translation : public TranslationBase<Translation<Dimension, Scalar, From, To>, From, To> {
  friend TransformationTraits<Translation<Dimension, Scalar, From, To>, PositionPoint<Dimension, Scalar, From>>;

 public:
  using Impl = PositionVector<Dimension, Scalar, From>;  // TODO(remod): Use eigen directly? Interface should be consistent with Quaternion

  explicit Translation(Impl impl) : impl_(impl) {}

 private:
  Impl impl_;
};

template <int Dimension, typename From, typename To, typename Scalar>
struct TransformationTraits<Translation<Dimension, Scalar, From, To>, PositionVector<Dimension, Scalar, From>> {
  static auto transform(const Translation<Dimension, Scalar, From, To>& /*transformation*/,
                        const PositionVector<Dimension, Scalar, From>& object) {
    return PositionVector<Dimension, Scalar, To>(object.getValues());
  }
};

template <int Dimension, typename From, typename To, typename Scalar>
struct TransformationTraits<Translation<Dimension, Scalar, From, To>, PositionPoint<Dimension, Scalar, From>> {
  static auto transform(const Translation<Dimension, Scalar, From, To>& transformation,
                        const PositionPoint<Dimension, Scalar, From>& object) {
    return PositionPoint<Dimension, Scalar, To>((transformation.impl_ + object).getValues());
  }
};

template <typename Scalar, typename From, typename To>
using Translation2D = Translation<2, Scalar, From, To>;

template <typename Scalar, typename From, typename To>
using Translation3D = Translation<3, Scalar, From, To>;

}  // namespace wuzp
