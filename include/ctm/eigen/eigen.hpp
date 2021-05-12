#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "ctm/core/QuantityMatrix.hpp"
#include "ctm/core/traits.hpp"
#include "ctm/trafo/Transformation.hpp"
#include "ctm/trafo/Translation.hpp"

namespace ctm {

template <typename Scalar, int Rows, int Cols>
struct Sum<Eigen::Matrix<Scalar, Rows, Cols>, Eigen::Matrix<Scalar, Rows, Cols>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>() + std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>());
};

template <typename Scalar, int Rows, int Cols>
struct Difference<Eigen::Matrix<Scalar, Rows, Cols>, Eigen::Matrix<Scalar, Rows, Cols>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>() - std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>());
};

template <typename Scalar, int Rows, int Cols>
struct Product<Eigen::Matrix<Scalar, Rows, Cols>, Eigen::Matrix<Scalar, Rows, Cols>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>() * std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>());
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

template <typename Scalar, int Rows, int Cols, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<Eigen::Matrix<Scalar, Rows, Cols>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  // Use decltype/declval to allow for lazy evaluation.
  using type =
      decltype(std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>().template block<BlockRows, BlockCols>(BlockRowId, BlockColId));
};

}  // namespace ctm
