#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "ctm/traits.hpp"
#include "ctm/Transformation.hpp"

namespace ctm {

template <typename Scalar, int Rows, int Cols>
struct Sum<Eigen::Matrix<Scalar, Rows, Cols>, Eigen::Matrix<Scalar, Rows, Cols>> {
  // TODO(remod): Enable lazy evaluation.
  using type = Eigen::Matrix<Scalar, Rows, Cols>;
};

template <typename From, typename To, typename Scalar>
using ProperRigidTransformationEigen = ProperRigidTransformation<From, To, Eigen::Quaternion<Scalar>, Eigen::Matrix<Scalar, 3, 1>>;

template <typename From, typename To, typename Scalar>
struct TransformationTraits<ProperRigidTransformationEigen<From, To, Scalar>, Eigen::Matrix<Scalar, 3, 1>> {
  static Eigen::Matrix<Scalar, 3, 1> transform(const ProperRigidTransformationEigen<From, To, Scalar>& transformation, const Eigen::Matrix<Scalar, 3, 1>& object) {
    return transformation.getRotation() * object + transformation.getTranslation();
  }
};

}  // namespace ctm
