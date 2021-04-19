#pragma once

#include "ctm/Position.hpp"
#include "ctm/QuantityMatrix.hpp"
#include "ctm/Rotation.hpp"
#include "ctm/Transformation.hpp"
#include "ctm/Translation.hpp"
#include "ctm/boost.hpp"
#include "ctm/eigen.hpp"
#include "ctm/traits.hpp"

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

template <typename Scalar, typename From, typename To>
class Quaternion
    : public QuaternionBase<Quaternion<Scalar, From, To>, From, To, boost::units::quantity<boost::units::si::dimensionless, Scalar>> {
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

template <int Dimension, typename Scalar, typename Frame>
using Translation = TranslationTemplate<Dimension, Eigen::Matrix<Scalar, Dimension, 1>, Frame, boost::units::si::length>;

template <typename Scalar, typename Frame>
using Translation2D = Translation<2, Scalar, Frame>;

template <typename Scalar, typename Frame>
using Translation3D = Translation<3, Scalar, Frame>;

template <int Dimension, typename From, typename To, typename Rotation, typename TranslationScalar>
using ProperRigidTransformation = ProperRigidTransformationTemplate<From, To, Rotation, Translation<Dimension, TranslationScalar, To>>;

template <typename From, typename To, typename Scalar>
using ProperRigidTransformation2D = ProperRigidTransformation<2, From, To, Scalar, Scalar>;

template <typename From, typename To, typename Scalar>
using ProperRigidTransformation3D = ProperRigidTransformation<3, From, To, Quaternion<Scalar, From, To>, Scalar>;

template <typename From, typename To, typename Scalar>
struct TransformationTraits<Quaternion<Scalar, From, To>, Position<3, Scalar, From>> {
  static auto transform(const Quaternion<Scalar, From, To>& transformation, const Position<3, Scalar, From>& object) {
    return Position<3, Scalar, To>(transformation.impl_ * object.getValues());
  }
};

template <int Dimension, typename From, typename To, typename Scalar>
struct TransformationTraits<ProperRigidTransformation3D<From, To, Scalar>, Position<Dimension, Scalar, From>> {
  static auto transform(const ProperRigidTransformation3D<From, To, Scalar>& transformation,
                        const Position<Dimension, Scalar, From>& object) {
    return transformation.getRotation().transform(object) + transformation.getTranslation();
  }
};

}  // namespace ctm
