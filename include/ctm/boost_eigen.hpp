#pragma once

#include "ctm/Position.hpp"
#include "ctm/QuantityMatrix.hpp"
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

template <int Dimension, typename Scalar, typename Frame>
using Translation = TranslationTemplate<Dimension, Eigen::Matrix<Scalar, Dimension, 1>, Frame, boost::units::si::length>;

template <typename Scalar, typename Frame>
using Translation2D = Translation<2, Scalar, Frame>;

template <typename Scalar, typename Frame>
using Translation3D = Translation<3, Scalar, Frame>;

template <int Dimension, typename From, typename To, typename Scalar>
using ProperRigidTransformation =
    ProperRigidTransformationTemplate<From, To, Eigen::Quaternion<Scalar>, Translation<Dimension, Scalar, To>>;

template <typename From, typename To, typename Scalar>
using ProperRigidTransformation2D = ProperRigidTransformation<2, From, To, Scalar>;

template <typename From, typename To, typename Scalar>
using ProperRigidTransformation3D = ProperRigidTransformation<3, From, To, Scalar>;

template <int Dimension, typename From, typename To, typename Scalar>
struct TransformationTraits<ProperRigidTransformation<Dimension, From, To, Scalar>, Position<Dimension, Scalar, From>> {
  static auto transform(const ProperRigidTransformation<Dimension, From, To, Scalar>& transformation,
                        const Position<Dimension, Scalar, From>& object) {
    return QuantityMatrix<Eigen::Matrix<Scalar, Dimension, 1>, TypeMatrix<Dimension, 1, To>,
                          TypeMatrix<Dimension, 1, boost::units::si::length>>(transformation.getRotation() * object.getValues()) +
           transformation.getTranslation();
  }
};

}  // namespace ctm
