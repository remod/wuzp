#pragma once

#include <type_traits>
#include <utility>

#include <blitz/array.h>

#include "wuzp/core/ValuesWrapper.hpp"

#include "wuzp_blitz/internal/comma_operations.hpp"

namespace wuzp_blitz {

template <typename ValueT>
using ValueMatrix = wuzp::ValuesWrapper<blitz::Array<ValueT, 2>>;

using ValueMatrixI = ValueMatrix<int>;
using ValueMatrixF = ValueMatrix<float>;
using ValueMatrixD = ValueMatrix<double>;

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

}  // namespace wuzp_blitz

namespace wuzp {

template <typename ValueT, int Rows, int Cols, typename Arg0, typename... Args>
void initialize(wuzp_blitz::ValueMatrix<ValueT>& values, Arg0&& arg0, Args&&... args) {
  Eigen::Matrix<ValueT> implementation;
  wuzp_blitz::internal::spliceCommas((implementation << std::forward<Arg0>(arg0)), std::forward<Args>(args)...);
  values.setImplementation(implementation);
}

template <typename T>
struct AccessValues<SpecializationDummy<T>> {
  template <typename ValueT_>
  using Value = wuzp_blitz::Value<ValueT_>;
  using ValueI = wuzp_blitz::ValueI;
  using ValueF = wuzp_blitz::ValueF;
  using ValueD = wuzp_blitz::ValueD;
};

}  // namespace wuzp
