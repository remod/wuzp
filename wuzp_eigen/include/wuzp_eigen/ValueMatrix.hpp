#pragma once

#include <type_traits>
#include <utility>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/LU>

#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/traits.hpp"

#include "wuzp_eigen/internal/comma_operations.hpp"

namespace wuzp_eigen {

// TODO: Move this typedef into wuzp itself?
template <typename ValueT, int Rows, int Cols>
using ValueMatrix = wuzp::ValuesWrapper<wuzp::MatrixSize<Rows, Cols>, ValueT, Eigen::Array<ValueT, Rows, Cols>>;

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

template <typename ImplementationT_, int... SizesV_>
struct ReserveMemory<ImplementationT_, MatrixSize<SizesV_...>> {
  static void run(ImplementationT_& /*impl*/) {}
};

template <typename ImplementationT_, typename Arg0, typename... Args>
struct Initialize<ImplementationT_, std::tuple<Arg0, Args...>> {
  static void run(ImplementationT_& impl, Arg0&& arg0, Args&&... args) {
    wuzp_eigen::internal::spliceCommas((impl << std::forward<Arg0>(arg0)), std::forward<Args>(args)...);
  }
};

template <typename T>
struct AccessValues<AccessImplementation<T>> {
  template <typename ValueT_>
  using Value = wuzp_eigen::Value<ValueT_>;
  using ValueI = wuzp_eigen::ValueI;
  using ValueF = wuzp_eigen::ValueF;
  using ValueD = wuzp_eigen::ValueD;
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Comparison<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static bool equal(const ImplementationLhsT_& lhs, const ImplementationRhsT_& rhs) { return lhs.matrix() == rhs.matrix(); }
};

template <typename ImplementationT_>
struct Transpose<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return impl.transpose(); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Sum<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) { return implLhs + implRhs; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Difference<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) { return implLhs - implRhs; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <typename ImplementationT_>
struct Negation<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return -impl; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationLhsT_, typename ValueRhsT_>
struct Product<AccessImplementation<ImplementationLhsT_>, ValueRhsT_> {
  static auto value(const ImplementationLhsT_& implLhs, const ValueRhsT_ valueRhs) { return implLhs * valueRhs; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ValueRhsT_>()));
};
template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Product<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) { return implLhs * implRhs; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct TensorProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) {
    // TODO(remod): implement
    return implLhs * implRhs;
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Contraction<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) {
    return (implLhs.matrix() * implRhs.matrix()).array();
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct CrossProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) {
    // Evaluate expression to avoid returning a temporary object.
    return (implLhs.matrix().cross(implRhs.matrix())).array().eval();
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Division<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) { return implLhs / implRhs; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};
template <typename ImplementationLhsT_, typename ValueRhsT_>
struct Division<AccessImplementation<ImplementationLhsT_>, ValueRhsT_> {
  static auto value(const ImplementationLhsT_& implLhs, const ValueRhsT_ valueRhs) { return implLhs / valueRhs; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ValueRhsT_>()));
};

template <typename ImplementationT_>
struct Inverse<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return impl.inverse(); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationT_>
struct TensorInverse<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return impl.matrix().inverse().array(); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationT_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<AccessImplementation<ImplementationT_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  static auto value(const ImplementationT_& impl) { return impl.template block<BlockRows, BlockCols>(BlockRowId, BlockColId); }
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
