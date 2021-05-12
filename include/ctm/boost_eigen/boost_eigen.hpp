#pragma once

#include "ctm/Position.hpp"
#include "ctm/Vector.hpp"
#include "ctm/boost/boost.hpp"
#include "ctm/core/FramedQuantityMatrix.hpp"
#include "ctm/core/QuantityMatrix.hpp"
#include "ctm/core/traits.hpp"
#include "ctm/eigen/eigen.hpp"
#include "ctm/trafo/Rotation.hpp"
#include "ctm/trafo/Transformation.hpp"
#include "ctm/trafo/Translation.hpp"

namespace ctm {

// template <int Rows, int Cols, typename Scalar, typename... Frames, typename... Units>
// using QuantityMatrix = QuantityMatrixTemplate<Eigen::Matrix<Scalar, Rows, Cols>, TypeMatrix<Rows, Cols, Frames...>, TypeMatrix<Rows,
// Cols, Units...>>;

template <int Dimension, typename Scalar, typename Frame>
using Position = PositionTemplate<Dimension, Eigen::Matrix<Scalar, Dimension, 1>, Frame, boost::units::absolute<boost::units::si::length>>;

template <typename Scalar, typename Frame>
using Position2D = Position<2, Scalar, Frame>;

template <typename Scalar, typename Frame>
using Position3D = Position<3, Scalar, Frame>;

template <int Dimension, typename Scalar, typename Frame>
using Vector = VectorTemplate<Dimension, Eigen::Matrix<Scalar, Dimension, 1>, Frame, boost::units::si::length>;

template <typename Scalar, typename Frame>
using Vector2D = Vector<2, Scalar, Frame>;

template <typename Scalar, typename Frame>
using Vector3D = Vector<3, Scalar, Frame>;

template <typename Scalar, typename From, typename To>
class Quaternion
    : public QuaternionBase<Quaternion<Scalar, From, To>, From, To, boost::units::quantity<boost::units::si::dimensionless, Scalar>> {
  // TODO(remod): Using friend is non-extendable. Change this.
  friend TransformationTraits<Quaternion<Scalar, From, To>, Vector<3, Scalar, From>>;
  friend TransformationTraits<Quaternion<Scalar, From, To>, Position<3, Scalar, From>>;

 public:
  using DimensionlessQuantity = boost::units::quantity<boost::units::si::dimensionless, Scalar>;

  template <typename... Ts>
  explicit Quaternion(Ts... args) : impl_(args...) {}

 protected:
  DimensionlessQuantity wDerived() const { return impl_.w(); }

 private:
  Eigen::Quaternion<Scalar> impl_;
};

template <int Dimension, typename Scalar, typename From, typename To>
class Translation : public TranslationBase<Translation<Dimension, Scalar, From, To>, From, To> {
  friend TransformationTraits<Translation<Dimension, Scalar, From, To>, Position<Dimension, Scalar, From>>;

 public:
  using Impl = Vector<Dimension, Scalar, From>;  // TODO(remod): Use eigen directly? Interface should be consistent with Quaternion

  explicit Translation(Impl impl) : impl_(impl) {}

 private:
  Impl impl_;
};

template <typename Scalar, typename From, typename To>
using Translation2D = Translation<2, Scalar, From, To>;

template <typename Scalar, typename From, typename To>
using Translation3D = Translation<3, Scalar, From, To>;

template <int Dimension, typename From, typename Intermediate, typename To, typename Rotation, typename TranslationScalar>
using ProperRigidTransformation =
    ProperRigidTransformationTemplate<From, Intermediate, Rotation, Translation<Dimension, TranslationScalar, Intermediate, To>>;

template <typename From, typename Intermediate, typename To, typename Scalar>
using ProperRigidTransformation2D = ProperRigidTransformation<2, From, Intermediate, To, Scalar, Scalar>;

template <typename From, typename Intermediate, typename To, typename Scalar>
using ProperRigidTransformation3D = ProperRigidTransformation<3, From, Intermediate, To, Quaternion<Scalar, From, Intermediate>, Scalar>;

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

template <typename From, typename To, typename Scalar>
struct TransformationTraits<Quaternion<Scalar, From, To>, Vector<3, Scalar, From>> {
  static auto transform(const Quaternion<Scalar, From, To>& transformation, const Vector<3, Scalar, From>& object) {
    return Vector<3, Scalar, To>(transformation.impl_ * object.getValues());
  }
};

template <typename From, typename To, typename Scalar>
struct TransformationTraits<Quaternion<Scalar, From, To>, Position<3, Scalar, From>> {
  static auto transform(const Quaternion<Scalar, From, To>& transformation, const Position<3, Scalar, From>& object) {
    return Position<3, Scalar, To>(transformation.impl_ * object.getValues());
  }
};

template <typename From, typename Intermediate, typename To, typename Scalar, typename Object>
struct TransformationTraits<ProperRigidTransformation3D<From, Intermediate, To, Scalar>, Object> {
  static auto transform(const ProperRigidTransformation3D<From, Intermediate, To, Scalar>& transformation, const Object& object) {
    return transformation.getTranslation().transform(transformation.getRotation().transform(object));
  }
};

}  // namespace ctm
