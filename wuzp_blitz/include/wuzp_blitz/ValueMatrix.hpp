#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <utility>

#include <blitz/array.h>
#include <blitz/range.h>

#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"

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

template <typename ImplementationT_, int... SizesV_>
struct ReserveMemory<ImplementationT_, MatrixSize<SizesV_...>> {
  static void run(ImplementationT_& impl) { impl.resize(SizesV_...); }
};

template <typename ImplementationT_, typename Arg0, typename... Args>
struct Initialize<ImplementationT_, std::tuple<Arg0, Args...>> {
  static void run(ImplementationT_& impl, Arg0&& arg0, Args&&... args) {
    applyCommas((impl = std::forward<Arg0>(arg0)), std::forward<Args>(args)...);
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

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct Comparison<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static bool equal(const ImplementationLhsT_& lhs, const ImplementationRhsT_& rhs) {
    return blitz::all(blitz::Array<bool, 2>(lhs == rhs));
  }
  // Relative comparison, like Eigen's isApprox: |lhs - rhs| <= tolerance * min(|lhs|, |rhs|), with the Frobenius norm.
  static bool approxEqual(const ImplementationLhsT_& lhs, const ImplementationRhsT_& rhs,
                          const typename ImplementationLhsT_::T_numtype tolerance) {
    const auto differenceNorm = std::sqrt(blitz::sum(blitz::pow2(lhs - rhs)));
    const auto lhsNorm = std::sqrt(blitz::sum(blitz::pow2(lhs)));
    const auto rhsNorm = std::sqrt(blitz::sum(blitz::pow2(rhs)));
    return differenceNorm <= tolerance * std::min(lhsNorm, rhsNorm);
  }
};

template <typename ImplementationT_>
struct Transpose<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return impl.transpose(1, 0); }
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
  static_assert(
      FailOnInstantiation<TensorProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>>::value,
      "Tensor product is not implemented for blitz.");
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct InnerProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) {
    // Blitz++ widens sums (e.g. of int to long). Keep the value type that ValuesWrapper declares for products, like Eigen.
    using ValueT =
        typename Product<ValueType<typename ImplementationLhsT_::T_numtype>, ValueType<typename ImplementationRhsT_::T_numtype>>::type;
    return blitz::cast<ValueT>(
        blitz::sum(implLhs(blitz::tensor::i, blitz::tensor::k) * implRhs(blitz::tensor::k, blitz::tensor::j), blitz::tensor::k));
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct OuterProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) {
    return implLhs(blitz::tensor::i, blitz::tensor::j) * implRhs(blitz::tensor::i, blitz::tensor::j);
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <typename ImplementationLhsT_, typename ImplementationRhsT_>
struct CrossProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) {
    static_assert(std::is_same_v<typename ImplementationLhsT_::T_numtype, typename ImplementationRhsT_::T_numtype>,
                  "Numeric types do not match.");
    // The core only calls this for 3x1 matrices: CrossProduct<MatrixSize> rejects other sizes at compile time.
    blitz::Array<typename ImplementationLhsT_::T_numtype, 2> result{3, 1};
    result(0, 0) = implLhs(1, 0) * implRhs(2, 0) - implLhs(2, 0) * implRhs(1, 0);
    result(1, 0) = implLhs(2, 0) * implRhs(0, 0) - implLhs(0, 0) * implRhs(2, 0);
    result(2, 0) = implLhs(0, 0) * implRhs(1, 0) - implLhs(1, 0) * implRhs(0, 0);
    return result;
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
struct Norm<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return std::sqrt(SquaredNorm<AccessImplementation<ImplementationT_>>::value(impl)); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationT_>
struct SquaredNorm<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return blitz::sum(impl * impl); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationT_>
struct Determinant<AccessImplementation<ImplementationT_>> {
  static_assert(FailOnInstantiation<Inverse<AccessImplementation<ImplementationT_>>>::value, "Determinant is not implemented for blitz.");
};

template <typename ImplementationT_>
struct Inverse<AccessImplementation<ImplementationT_>> {
  static_assert(FailOnInstantiation<Inverse<AccessImplementation<ImplementationT_>>>::value, "Inverse is not implemented for blitz.");
};

template <typename ImplementationT_>
struct TensorInverse<AccessImplementation<ImplementationT_>> {
  static_assert(FailOnInstantiation<Inverse<AccessImplementation<ImplementationT_>>>::value,
                "Tensor inverse is not implemented for blitz.");
};

template <typename ImplementationT_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<AccessImplementation<ImplementationT_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  static auto value(const ImplementationT_& impl) {
    return impl(blitz::Range(BlockRowId, BlockRowId + BlockRows - 1), blitz::Range(BlockColId, BlockColId + BlockCols - 1));
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationT_>
struct Zero<AccessImplementation<ImplementationT_>> {
  static void run(ImplementationT_& impl) { impl = 0; }
};

template <typename ImplementationT_>
struct Random<AccessImplementation<ImplementationT_>> {
  // Uniformly distributed in [-1, 1], like Eigen's setRandom().
  static void run(ImplementationT_& impl) {
    using ValueT = typename ImplementationT_::T_numtype;
    for (int row = 0; row < impl.rows(); ++row) {
      for (int col = 0; col < impl.cols(); ++col) {
        impl(row, col) =
            static_cast<ValueT>(-1) + static_cast<ValueT>(2) * static_cast<ValueT>(std::rand()) / static_cast<ValueT>(RAND_MAX);
      }
    }
  }
};

template <typename ImplementationT_>
struct Evaluate<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return blitz::Array<typename ImplementationT_::T_numtype, 2>(impl); }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

}  // namespace wuzp
