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

namespace internal {

//! The array type of a Blitz++ array or expression.
template <typename ImplementationT_>
using ArrayOf = blitz::Array<typename ImplementationT_::T_numtype, 2>;

template <typename ImplementationT_>
inline constexpr bool isArray = std::is_same_v<ImplementationT_, ArrayOf<ImplementationT_>>;

//! The values of a Blitz++ array or expression in a new array, which shares them with no other array. Blitz++'s copy
//! constructor makes a shared view of an array instead (see "Referencing another array" in
//! https://github.com/blitzpp/blitz/blob/main/doc/arrays-ctors.texi), and so do transpose() and slicing.
template <typename ImplementationT_>
ArrayOf<ImplementationT_> copyToArray(const ImplementationT_& impl) {
  if constexpr (isArray<ImplementationT_>) {
    // Unlike impl.copy(), which keeps the storage order of a transposed view, this stores the copy row by row, like all other
    // arrays of the backend.
    ArrayOf<ImplementationT_> result(impl.extent(0), impl.extent(1));
    result = impl;
    return result;
  } else {
    // Evaluates the expression into a new array.
    return ArrayOf<ImplementationT_>(impl);
  }
}

//! A Blitz++ array as it is, or an expression evaluated into a new array: some members, e.g. transpose(), only exist for
//! arrays.
template <typename ImplementationT_>
decltype(auto) asArray(const ImplementationT_& impl) {
  if constexpr (isArray<ImplementationT_>) {
    return (impl);
  } else {
    return ArrayOf<ImplementationT_>(impl);
  }
}

}  // namespace internal

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

// Blitz++'s copy constructor shares the values of an array, so arrays are copied into new arrays. Copies of expressions refer
// to the same operands, like the expressions themselves.
template <typename ImplementationT_>
struct Copy<AccessImplementation<ImplementationT_>> {
  static ImplementationT_ value(const ImplementationT_& impl) {
    if constexpr (wuzp_blitz::internal::isArray<ImplementationT_>) {
      return wuzp_blitz::internal::copyToArray(impl);
    } else {
      return impl;
    }
  }
};

// Blitz++ arrays cannot be moved, and their copy constructor shares the values: the moved array takes over the values with
// reference(), and the moved-from array is emptied with free() (see "Array members" in
// https://github.com/blitzpp/blitz/blob/main/doc/arrays-members.texi), so that no two arrays share the values.
template <typename ImplementationT_>
struct Move<AccessImplementation<ImplementationT_>> {
  static ImplementationT_ construct(ImplementationT_& source) {
    if constexpr (wuzp_blitz::internal::isArray<ImplementationT_>) {
      ImplementationT_ result;
      result.reference(source);
      source.free();
      return result;
    } else {
      return source;
    }
  }
  // Only arrays are assigned: the core rejects assigning to unevaluated values, i.e. expressions.
  static void assign(ImplementationT_& target, ImplementationT_& source) {
    target.reference(source);
    source.free();
  }
};

template <typename ImplementationT_>
struct Transpose<AccessImplementation<ImplementationT_>> {
  // Blitz++'s transpose() is a view that shares the values of the array, so the transpose is copied: it does not change with the
  // original. Expressions have no transpose(), so they are evaluated first.
  static auto value(const ImplementationT_& impl) {
    return wuzp_blitz::internal::copyToArray(wuzp_blitz::internal::asArray(impl).transpose(1, 0));
  }
  // Not lazy, see above.
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
    // The result is evaluated: Blitz++'s lazy reduction leaves a member uninitialized until it is evaluated, so copying it
    // reads an uninitialized value (GCC warns about this in optimized builds).
    // Expressions are evaluated first, for their extents and the index placeholders.
    const auto& lhs = wuzp_blitz::internal::asArray(implLhs);
    const auto& rhs = wuzp_blitz::internal::asArray(implRhs);
    blitz::Array<ValueT, 2> result(lhs.extent(0), rhs.extent(1));
    result = blitz::cast<ValueT>(
        blitz::sum(lhs(blitz::tensor::i, blitz::tensor::k) * rhs(blitz::tensor::k, blitz::tensor::j), blitz::tensor::k));
    return result;
  }
  // Not lazy, see above.
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
    // The core only calls this for 3x1 matrices: CrossProduct<MatrixSize> rejects other sizes at compile time. Expressions are
    // evaluated first, for the element access.
    const auto& lhs = wuzp_blitz::internal::asArray(implLhs);
    const auto& rhs = wuzp_blitz::internal::asArray(implRhs);
    blitz::Array<typename ImplementationLhsT_::T_numtype, 2> result{3, 1};
    result(0, 0) = lhs(1, 0) * rhs(2, 0) - lhs(2, 0) * rhs(1, 0);
    result(1, 0) = lhs(2, 0) * rhs(0, 0) - lhs(0, 0) * rhs(2, 0);
    result(2, 0) = lhs(0, 0) * rhs(1, 0) - lhs(1, 0) * rhs(0, 0);
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
  static auto value(const ImplementationT_& impl) {
    // Keep the value type, like Eigen: std::sqrt returns double for integers.
    return static_cast<typename ImplementationT_::T_numtype>(std::sqrt(SquaredNorm<AccessImplementation<ImplementationT_>>::value(impl)));
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <typename ImplementationT_>
struct SquaredNorm<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) {
    // Keep the value type, like Eigen: Blitz++ widens sums, e.g. of int to long and of float to double.
    return static_cast<typename ImplementationT_::T_numtype>(blitz::sum(impl * impl));
  }
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
  // A slice of an array is a view that shares its values, like transpose(), so the block is copied.
  static auto value(const ImplementationT_& impl) {
    return wuzp_blitz::internal::copyToArray(wuzp_blitz::internal::asArray(impl)(blitz::Range(BlockRowId, BlockRowId + BlockRows - 1),
                                                                                 blitz::Range(BlockColId, BlockColId + BlockCols - 1)));
  }
  // Not lazy, see above.
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
  // A new array, also for an array: the result of evaluate() does not share the values of the original.
  static auto value(const ImplementationT_& impl) { return wuzp_blitz::internal::copyToArray(impl); }
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

}  // namespace wuzp
