#pragma once

#include "wuzp/core/ValueMatrixWrapper.hpp"

namespace wuzp_value_matrix {

template <typename ValueT_>
class Scalar {
 private:
  ValueT_ value_;
};

template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp::ValueMatrixWrapper<Scalar<ValueT>>;

}  // namespace wuzp_value_matrix

namespace wuzp {

//template <typename ValueT, int Rows, int Cols, typename Arg0, typename... Args>
//void initialize(ValueMatrixWrapper<Eigen::Matrix<ValueT, Rows, Cols>>& valueMatrix, Arg0&& arg0, Args&&... args) {
//  Eigen::Matrix<ValueT, Rows, Cols> implementation;
//  wuzp_eigen::internal::spliceCommas((implementation << std::forward<Arg0>(arg0)), std::forward<Args>(args)...);
//  valueMatrix.setImplementation(implementation);
//}

}  // namespace wuzp
