#pragma once

#include <type_traits>
#include <utility>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>

#include "wuzp/core/ValueMatrixWrapper.hpp"

#include "wuzp_eigen/internal/comma_operations.hpp"

namespace wuzp_eigen {

template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp::ValueMatrixWrapper<Eigen::Matrix<ValueT, Rows, Cols>>;

}  // namespace wuzp_eigen

namespace wuzp {

template <typename ValueT, int Rows, int Cols, typename Arg0, typename... Args>
void initialize(ValueMatrixWrapper<Eigen::Matrix<ValueT, Rows, Cols>>& valueMatrix, Arg0&& arg0, Args&&... args) {
  Eigen::Matrix<ValueT, Rows, Cols> implementation;
  wuzp_eigen::internal::spliceCommas((implementation << std::forward<Arg0>(arg0)), std::forward<Args>(args)...);
  valueMatrix.setImplementation(implementation);
}

}  // namespace wuzp
