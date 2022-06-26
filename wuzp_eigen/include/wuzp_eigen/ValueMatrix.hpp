#pragma once

#include <type_traits>
#include <utility>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>

#include "wuzp/core/ValuesWrapper.hpp"

#include "wuzp_eigen/internal/comma_operations.hpp"

namespace wuzp_eigen {

template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp::ValuesWrapper<Eigen::Matrix<ValueT, Rows, Cols>>;

template <int Rows, int Cols>
using ValueMatrixI = ValueMatrix<int, Rows, Cols>;
template <int Rows, int Cols>
using ValueMatrixF = ValueMatrix<float, Rows, Cols>;
template <int Rows, int Cols>
using ValueMatrixD = ValueMatrix<double, Rows, Cols>;

template <typename ValueT, int Rows>
using ValueVector = ValueMatrix<ValueT, Rows, 1>;

template <int Rows>
using ValueVectorI = ValueVector<int, Rows>;
template <int Rows>
using ValueVectorF = ValueVector<float, Rows>;
template <int Rows>
using ValueVectorD = ValueVector<double, Rows>;

template <typename ValueT>
using Value = ValueMatrix<ValueT, 1, 1>;

using ValueI = Value<int>;
using ValueF = Value<float>;
using ValueD = Value<double>;

}  // namespace wuzp_eigen

namespace wuzp {

template <typename ValueT, int Rows, int Cols, typename Arg0, typename... Args>
void initialize(wuzp_eigen::ValueMatrix<ValueT, Rows, Cols>& values, Arg0&& arg0, Args&&... args) {
  Eigen::Matrix<ValueT, Rows, Cols> implementation;
  wuzp_eigen::internal::spliceCommas((implementation << std::forward<Arg0>(arg0)), std::forward<Args>(args)...);
  values.setImplementation(implementation);
}

template <typename T>
struct AccessValues<SpecializationDummy<T>> {
  template <typename ValueT_>
  using Value = wuzp_eigen::Value<ValueT_>;
  using ValueI = wuzp_eigen::ValueI;
  using ValueF = wuzp_eigen::ValueF;
  using ValueD = wuzp_eigen::ValueD;
};

}  // namespace wuzp
