#pragma once

#include <type_traits>
#include <utility>

#include <blitz/array.h>
#include <blitz/range.h>

#include "wuzp/core/ValuesWrapper.hpp"

#include "wuzp_blitz/internal/comma_operations.hpp"

namespace wuzp_blitz {

template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp::ValuesWrapper<wuzp::MatrixSize<Rows, Cols>, blitz::Array<ValueT, 2>>;

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

}  // namespace wuzp_blitz

namespace wuzp {

template <typename ValueT, int Rows, int Cols, typename Arg0, typename... Args>
void initialize(wuzp_blitz::ValueMatrix<ValueT, Rows, Cols>& values, Arg0&& arg0, Args&&... args) {
  blitz::Array<ValueT, 2> implementation(Rows, Cols);
  wuzp_blitz::internal::spliceCommas((implementation = std::forward<Arg0>(arg0)), std::forward<Args>(args)...);
  values.setImplementation(implementation);
}

template <typename T>
struct AccessValues<AccessImplementation<T>> {
  template <typename ValueT_>
  using Value = wuzp_blitz::Value<ValueT_>;
  using ValueI = wuzp_blitz::ValueI;
  using ValueF = wuzp_blitz::ValueF;
  using ValueD = wuzp_blitz::ValueD;
};

template <typename ImplementationT_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<AccessExtension<wuzp::BlockTag, ImplementationT_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  static auto value(const ImplementationT_& impl) {
    return impl(blitz::Range(BlockRowId, BlockRowId + BlockRows), blitz::Range(BlockColId, BlockColId + BlockCols));
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationT_>
struct Zero<AccessExtension<wuzp::ZeroTag, ImplementationT_>> {
  static auto value() { return ImplementationT_::Zero(); }
};

template <typename ImplementationT_>
struct Random<AccessExtension<wuzp::RandomTag, ImplementationT_>> {
  static auto value() { return ImplementationT_::Random(); }
};

template <typename ImplementationT_>
struct Evaluate<AccessExtension<wuzp::EvaluateTag, ImplementationT_>> {
  using type = std::decay_t<decltype(std::declval<ImplementationT_>().eval())>;
};

}  // namespace wuzp
