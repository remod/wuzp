#pragma once

#include <type_traits>
#include <utility>

#include <blitz/array.h>
#include <blitz/range.h>

#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/traits.hpp"

#include "wuzp_blitz/internal/comma_operations.hpp"

namespace wuzp_blitz {

template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp::ValuesWrapper<wuzp::MatrixSize<Rows, Cols>, ValueT, blitz::Array<ValueT, 2>>;

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

template <typename ImplementationT_, int... SizesV_, typename Arg0, typename... Args>
struct Initialize<ImplementationT_, MatrixSize<SizesV_...>, Arg0, Args...> {
  static void run(ImplementationT_& impl, Arg0&& arg0, Args&&... args) {
    impl.resize(SizesV_...);
    wuzp_blitz::internal::spliceCommas((impl = std::forward<Arg0>(arg0)), std::forward<Args>(args)...);
  }
};

template <typename T>
struct AccessValues<AccessImplementation<T>> {
  template <typename ValueT_>
  using Value = wuzp_blitz::Value<ValueT_>;
  using ValueI = wuzp_blitz::ValueI;
  using ValueF = wuzp_blitz::ValueF;
  using ValueD = wuzp_blitz::ValueD;
};

template <typename ImplementationT_>
struct Transpose<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return impl.transpose(0, 1); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct CrossProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) { return implLhs.cross(implRhs); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <typename ImplementationT_>
struct Inverse<AccessImplementation<ImplementationT_>> {
  static_assert(FailOnInstantiation<Inverse<AccessImplementation<ImplementationT_>>>::value,
                "Matrix inversion is not implemented for blitz.");
};

template <typename ImplementationT_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<AccessImplementation<ImplementationT_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  static auto value(const ImplementationT_& impl) {
    return impl(blitz::Range(BlockRowId, BlockRowId + BlockRows), blitz::Range(BlockColId, BlockColId + BlockCols));
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationT_>
struct Zero<AccessImplementation<ImplementationT_>> {
  static auto value() { return ImplementationT_::Zero(); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationT_>
struct Random<AccessImplementation<ImplementationT_>> {
  static auto value() { return ImplementationT_::Random(); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationT_>
struct Evaluate<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return impl.eval(); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

}  // namespace wuzp
