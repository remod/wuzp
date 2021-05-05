#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "ctm/QuantityMatrix.hpp"
#include "ctm/Transformation.hpp"
#include "ctm/Translation.hpp"
#include "ctm/traits.hpp"

namespace ctm {

template <typename Scalar, int Rows, int Cols, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct GetBlock<Eigen::Matrix<Scalar, Rows, Cols>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<Eigen::Matrix<Scalar, Rows, Cols>>().block<BlockRows, BlockCols>(BlockRowId, BlockColId));
};

template <typename Scalar, int Rows, int Cols>
struct Sum<Eigen::Matrix<Scalar, Rows, Cols>, Eigen::Matrix<Scalar, Rows, Cols>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<Eigen::Matrix<Scalar, Rows, Cols>>() + std::declval<Eigen::Matrix<Scalar, Rows, Cols>>());
};

template <typename Scalar, int Rows, int Cols>
struct Transpose<Eigen::Matrix<Scalar, Rows, Cols>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>().transpose());
};

template <typename Scalar, int Rows, int Cols>
struct Inverse<Eigen::Matrix<Scalar, Rows, Cols>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>().inverse());
};

}  // namespace ctm
