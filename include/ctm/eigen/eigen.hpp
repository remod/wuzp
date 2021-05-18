#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "ctm/core/QuantityMatrix.hpp"
#include "ctm/core/traits.hpp"
#include "ctm/trafo/Transformation.hpp"
#include "ctm/trafo/Translation.hpp"

namespace ctm {
namespace eigen {

template <typename Lhs, typename Rhs>
struct ValuesSum {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<const Lhs>() + std::declval<const Rhs>());
};

template <typename Lhs, typename Rhs>
struct ValuesDifference {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<const Lhs>() - std::declval<const Rhs>());
};

template <typename Lhs, typename Rhs>
struct ValuesProduct {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<const Lhs>() * std::declval<const Rhs>());
};

}  // namespace eigen

template <typename Lhs, typename Rhs>
struct ValuesSum<Lhs, Rhs> {
  using type = typename eigen::ValuesSum<Lhs, Rhs>::type;
};

template <typename Lhs, typename Rhs>
struct ValuesDifference<Lhs, Rhs> {
  using type = typename eigen::ValuesDifference<Lhs, Rhs>::type;
};

template <typename Lhs, typename Rhs>
struct ValuesProduct<Lhs, Rhs> {
  using type = typename eigen::ValuesProduct<Lhs, Rhs>::type;
};

template <typename Scalar, int Rows, int Cols>
struct Transpose<Eigen::Matrix<Scalar, Rows, Cols>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>().transpose());
};

template <typename Scalar, int Rows, int Cols>
struct DotProduct<Eigen::Matrix<Scalar, Rows, Cols>, Eigen::Matrix<Scalar, Rows, Cols>> {
  // Use decltype/declval to allow for lazy evaluation.
  using type =
      decltype(std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>().dot(std::declval<const Eigen::Matrix<Scalar, Rows, Cols>>()));
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
