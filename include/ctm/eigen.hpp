#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "ctm/QuantityMatrix.hpp"
#include "ctm/Transformation.hpp"
#include "ctm/Translation.hpp"
#include "ctm/traits.hpp"

namespace ctm {

template <typename Scalar, int Rows, int Cols>
struct Sum<Eigen::Matrix<Scalar, Rows, Cols>, Eigen::Matrix<Scalar, Rows, Cols>> {
  using type = decltype(std::declval<Eigen::Matrix<Scalar, Rows, Cols>>() + std::declval<Eigen::Matrix<Scalar, Rows, Cols>>());
};

template <typename Scalar, int Rows, int Cols>
struct Transpose<Eigen::Matrix<Scalar, Rows, Cols>> {
  using type = decltype(std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>().transpose());
};

}  // namespace ctm
