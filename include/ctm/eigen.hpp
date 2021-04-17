#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "ctm/Matrix.hpp"
#include "ctm/Transformation.hpp"
#include "ctm/traits.hpp"

namespace ctm {

template <typename Scalar, int Rows, int Cols>
struct Sum<Eigen::Matrix<Scalar, Rows, Cols>, Eigen::Matrix<Scalar, Rows, Cols>> {
  // TODO(remod): Enable lazy evaluation.
  using type = Eigen::Matrix<Scalar, Rows, Cols>;
};

template <typename From, typename To, typename Scalar>
using ProperRigidTransformationEigen = ProperRigidTransformation<From, To, Eigen::Quaternion<Scalar>, Eigen::Matrix<Scalar, 3, 1>>;

template <typename From, typename To, typename Scalar, typename Frames, typename Units>
struct TransformationTraits<ProperRigidTransformationEigen<From, To, Scalar>, Matrix<Eigen::Matrix<Scalar, 3, 1>, Frames, Units>> {
  static auto transform(const ProperRigidTransformationEigen<From, To, Scalar>& transformation,
                        const Matrix<Eigen::Matrix<Scalar, 3, 1>, Frames, Units>& object) {
    return Matrix<Eigen::Matrix<Scalar, 3, 1>, TypeMatrix<3, 1, To>, Units>(transformation.getRotation() * object.getValues() +
                                                                            transformation.getTranslation());
  }
};

}  // namespace ctm
