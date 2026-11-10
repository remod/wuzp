#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include <blitz/array.h>
#include <blitz/range.h>

#include "wuzp/core/MatrixSize.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/internal/operator_traits.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp_blitz {

template <typename ValueT_, int RowsV_, int ColsV_>
using ValueMatrix = wuzp::ValuesWrapper<wuzp::MatrixSize<RowsV_, ColsV_>, ValueT_, blitz::Array<ValueT_, 2>>;

template <int RowsV_, int ColsV_>
using ValueMatrixI = ValueMatrix<int, RowsV_, ColsV_>;
template <int RowsV_, int ColsV_>
using ValueMatrixF = ValueMatrix<float, RowsV_, ColsV_>;
template <int RowsV_, int ColsV_>
using ValueMatrixD = ValueMatrix<double, RowsV_, ColsV_>;

template <typename ValueT_, int RowsV_>
using ValueVector = ValueMatrix<ValueT_, RowsV_, 1>;

template <int RowsV_>
using ValueVectorI = ValueVector<int, RowsV_>;
template <int RowsV_>
using ValueVectorF = ValueVector<float, RowsV_>;
template <int RowsV_>
using ValueVectorD = ValueVector<double, RowsV_>;

template <typename ValueT_>
using Value = ValueMatrix<ValueT_, 1, 1>;

using ValueI = Value<int>;
using ValueF = Value<float>;
using ValueD = Value<double>;

//! A quantity with a value of this backend, e.g. QuantityD<wuzp_units::Length>.
template <typename UnitT_>
using QuantityI = wuzp::Quantity<ValueI, UnitT_>;
template <typename UnitT_>
using QuantityF = wuzp::Quantity<ValueF, UnitT_>;
template <typename UnitT_>
using QuantityD = wuzp::Quantity<ValueD, UnitT_>;

namespace internal {

template <typename T>
struct IsBlitzArray : std::false_type {};
template <typename ValueT_>
struct IsBlitzArray<blitz::Array<ValueT_, 2>> : std::true_type {};
template <typename ExpressionT_>
struct IsBlitzArray<blitz::_bz_ArrayExpr<ExpressionT_>> : std::true_type {};

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

//! Swaps two rows of a matrix.
template <typename ValueT_>
void swapRows(blitz::Array<ValueT_, 2>& matrix, const int row, const int otherRow) {
  for (int col = 0; col < matrix.extent(1); ++col) {
    std::swap(matrix(row, col), matrix(otherRow, col));
  }
}

/**
 * The matrix inverse (first) and the determinant (second) of a square matrix, computed in the value type WorkT_ by
 * Gauss-Jordan elimination with partial pivoting, since Blitz++ provides neither (see "Backend support for inverses" in
 * DESIGN.md). The elimination reduces the matrix to the identity with row operations, which turn the identity into the inverse;
 * before eliminating a column, the row with the largest magnitude in it, at or below the diagonal, is swapped into the pivot
 * position, which keeps the elimination stable in practice. The determinant is the product of the pivots, with the sign of the
 * row swaps. See Golub and Van Loan, Matrix Computations (4th ed., 2013), sections 3.2 and 3.4, and Press et al., Numerical
 * Recipes (3rd ed., 2007), section 2.1. It costs about n^3 multiplications, which is negligible for the small matrices of wuzp.
 * A matrix is singular if a pivot is exactly zero, i.e. if a column is zero at and below the diagonal after the elimination of
 * the columns before it: then the determinant is zero, and every element of the inverse is NaN, like the non-finite elements
 * of Eigen's inverse of a singular matrix. A matrix that is singular only up to rounding has a tiny pivot instead: its inverse
 * has huge elements, like with Eigen.
 */
template <typename WorkT_, typename ImplementationT_>
std::pair<blitz::Array<WorkT_, 2>, WorkT_> gaussJordan(const ImplementationT_& impl) {
  const auto& matrix = asArray(impl);
  const int size = matrix.extent(0);
  blitz::Array<WorkT_, 2> reduced(size, size);
  reduced = blitz::cast<WorkT_>(matrix);
  blitz::Array<WorkT_, 2> inverse(size, size);
  inverse = WorkT_{0};
  for (int index = 0; index < size; ++index) {
    inverse(index, index) = WorkT_{1};
  }
  WorkT_ determinant{1};
  for (int col = 0; col < size; ++col) {
    int pivot = col;
    for (int row = col + 1; row < size; ++row) {
      if (std::abs(reduced(row, col)) > std::abs(reduced(pivot, col))) {
        pivot = row;
      }
    }
    if (reduced(pivot, col) == WorkT_{0}) {
      inverse = std::numeric_limits<WorkT_>::quiet_NaN();
      return {inverse, WorkT_{0}};
    }
    if (pivot != col) {
      swapRows(reduced, pivot, col);
      swapRows(inverse, pivot, col);
      determinant = -determinant;
    }
    const WorkT_ pivotValue = reduced(col, col);
    determinant *= pivotValue;
    // The columns before col are those of the identity already.
    for (int otherCol = col; otherCol < size; ++otherCol) {
      reduced(col, otherCol) /= pivotValue;
    }
    for (int otherCol = 0; otherCol < size; ++otherCol) {
      inverse(col, otherCol) /= pivotValue;
    }
    for (int row = 0; row < size; ++row) {
      if (row != col) {
        const WorkT_ factor = reduced(row, col);
        for (int otherCol = col; otherCol < size; ++otherCol) {
          reduced(row, otherCol) -= factor * reduced(col, otherCol);
        }
        for (int otherCol = 0; otherCol < size; ++otherCol) {
          inverse(row, otherCol) -= factor * inverse(col, otherCol);
        }
      }
    }
  }
  return {inverse, determinant};
}

}  // namespace internal

//! The implementation types of the backend: Blitz++'s two-dimensional arrays and the expressions of arrays, _bz_ArrayExpr (see
//! "Array expressions" in https://github.com/blitzpp/blitz/blob/main/doc/arrays-expr.texi). The traits of the backend are only
//! specialized for them, so that other value matrix backends can be used in the same program.
template <typename T>
concept BlitzArray = internal::IsBlitzArray<T>::value;

}  // namespace wuzp_blitz

namespace wuzp {

template <wuzp_blitz::BlitzArray ImplementationT_, int... SizesV_>
struct ReserveMemory<ImplementationT_, MatrixSize<SizesV_...>> {
  static void run(ImplementationT_& impl) { impl.resize(SizesV_...); }
};

template <wuzp_blitz::BlitzArray ImplementationT_, typename Arg0T_, typename... ArgsT_>
struct Initialize<ImplementationT_, std::tuple<Arg0T_, ArgsT_...>> {
  static void run(ImplementationT_& impl, Arg0T_&& arg0, ArgsT_&&... args) {
    applyCommas((impl = std::forward<Arg0T_>(arg0)), std::forward<ArgsT_>(args)...);
  }
};

// All matrices are two-dimensional arrays, whose size is only known at run time.
template <wuzp_blitz::BlitzArray ImplementationT_, typename ValueT_, int RowsV_, int ColsV_>
struct Rebind<AccessImplementation<ImplementationT_>, ValueT_, RowsV_, ColsV_> {
  using type = blitz::Array<ValueT_, 2>;
};

// Blitz++ accesses elements with the parenthesis operator (see "Indexing" in
// https://github.com/blitzpp/blitz/blob/main/doc/arrays-slicing.texi).
template <wuzp_blitz::BlitzArray ImplementationT_>
struct Coefficient<AccessImplementation<ImplementationT_>> {
  static auto get(const ImplementationT_& impl, const int row, const int col) { return impl(row, col); }
  // Only arrays are assigned to: the core rejects assigning to unevaluated values, i.e. expressions.
  static void set(ImplementationT_& impl, const int row, const int col, const typename ImplementationT_::T_numtype value) {
    impl(row, col) = value;
  }
};

template <wuzp_blitz::BlitzArray ImplementationLhsT_, wuzp_blitz::BlitzArray ImplementationRhsT_>
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
template <wuzp_blitz::BlitzArray ImplementationT_>
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
template <wuzp_blitz::BlitzArray ImplementationT_>
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

template <wuzp_blitz::BlitzArray ImplementationT_>
struct Transpose<AccessImplementation<ImplementationT_>> {
  // Blitz++'s transpose() is a view that shares the values of the array, so the transpose is copied: it does not change with the
  // original. Expressions have no transpose(), so they are evaluated first.
  static auto value(const ImplementationT_& impl) {
    return wuzp_blitz::internal::copyToArray(wuzp_blitz::internal::asArray(impl).transpose(1, 0));
  }
  // Not lazy, see above.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <wuzp_blitz::BlitzArray ImplementationLhsT_, wuzp_blitz::BlitzArray ImplementationRhsT_>
struct Sum<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>
    : internal::OperatorSum<ImplementationLhsT_, ImplementationRhsT_> {};

template <wuzp_blitz::BlitzArray ImplementationLhsT_, wuzp_blitz::BlitzArray ImplementationRhsT_>
struct Difference<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>
    : internal::OperatorDifference<ImplementationLhsT_, ImplementationRhsT_> {};

template <wuzp_blitz::BlitzArray ImplementationT_>
struct Negation<AccessImplementation<ImplementationT_>> : internal::OperatorNegation<ImplementationT_> {};

template <wuzp_blitz::BlitzArray ImplementationLhsT_, ArithmeticValue ValueRhsT_>
struct Product<AccessImplementation<ImplementationLhsT_>, ValueRhsT_> : internal::OperatorProduct<ImplementationLhsT_, ValueRhsT_> {};
template <wuzp_blitz::BlitzArray ImplementationLhsT_, wuzp_blitz::BlitzArray ImplementationRhsT_>
struct Product<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>
    : internal::OperatorProduct<ImplementationLhsT_, ImplementationRhsT_> {};

template <wuzp_blitz::BlitzArray ImplementationLhsT_, wuzp_blitz::BlitzArray ImplementationRhsT_>
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

template <wuzp_blitz::BlitzArray ImplementationLhsT_, wuzp_blitz::BlitzArray ImplementationRhsT_>
struct OuterProduct<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>> {
  static auto value(const ImplementationLhsT_& implLhs, const ImplementationRhsT_& implRhs) {
    return implLhs(blitz::tensor::i, blitz::tensor::j) * implRhs(blitz::tensor::i, blitz::tensor::j);
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationLhsT_>(), std::declval<const ImplementationRhsT_>()));
};

template <wuzp_blitz::BlitzArray ImplementationLhsT_, wuzp_blitz::BlitzArray ImplementationRhsT_>
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

template <wuzp_blitz::BlitzArray ImplementationLhsT_, wuzp_blitz::BlitzArray ImplementationRhsT_>
struct Division<AccessImplementation<ImplementationLhsT_>, AccessImplementation<ImplementationRhsT_>>
    : internal::OperatorDivision<ImplementationLhsT_, ImplementationRhsT_> {};
template <wuzp_blitz::BlitzArray ImplementationLhsT_, ArithmeticValue ValueRhsT_>
struct Division<AccessImplementation<ImplementationLhsT_>, ValueRhsT_> : internal::OperatorDivision<ImplementationLhsT_, ValueRhsT_> {};

template <wuzp_blitz::BlitzArray ImplementationT_>
struct Norm<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) {
    // Keep the value type, like Eigen: std::sqrt returns double for integers.
    return static_cast<typename ImplementationT_::T_numtype>(std::sqrt(SquaredNorm<AccessImplementation<ImplementationT_>>::value(impl)));
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <wuzp_blitz::BlitzArray ImplementationT_>
struct SquaredNorm<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) {
    // Keep the value type, like Eigen: Blitz++ widens sums, e.g. of int to long and of float to double.
    return static_cast<typename ImplementationT_::T_numtype>(blitz::sum(impl * impl));
  }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

// Blitz++ provides neither determinants nor matrix inverses, so the backend computes them by Gauss-Jordan elimination (see
// internal::gaussJordan()).

// The determinant of an integer matrix is an integer, like Eigen's: it is computed in double and rounded to the nearest integer,
// which is exact as long as the rounding errors of the elimination stay below 1/2, e.g. for small matrices of small integers.
template <wuzp_blitz::BlitzArray ImplementationT_>
struct Determinant<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) {
    using ValueT = typename ImplementationT_::T_numtype;
    if constexpr (std::is_floating_point_v<ValueT>) {
      return wuzp_blitz::internal::gaussJordan<ValueT>(impl).second;
    } else {
      return static_cast<ValueT>(std::llround(wuzp_blitz::internal::gaussJordan<double>(impl).second));
    }
  }
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

// The matrix inverse. The core rejects the inverses of integer matrices (see ValuesWrapper::inverse()).
template <wuzp_blitz::BlitzArray ImplementationT_>
struct Inverse<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) {
    return wuzp_blitz::internal::gaussJordan<typename ImplementationT_::T_numtype>(impl).first;
  }
  // Not lazy: the elimination computes the inverse into a new array.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <wuzp_blitz::BlitzArray ImplementationT_>
struct CwiseInverse<AccessImplementation<ImplementationT_>> {
  static auto value(const ImplementationT_& impl) { return static_cast<typename ImplementationT_::T_numtype>(1) / impl; }
  // Use decltype/declval to allow for lazy evaluation.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <wuzp_blitz::BlitzArray ImplementationT_, int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_>
struct Block<AccessImplementation<ImplementationT_>, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_> {
  // A slice of an array is a view that shares its values, like transpose(), so the block is copied.
  static auto value(const ImplementationT_& impl) {
    return wuzp_blitz::internal::copyToArray(wuzp_blitz::internal::asArray(impl)(
        blitz::Range(BlockRowIdV_, BlockRowIdV_ + BlockRowsV_ - 1), blitz::Range(BlockColIdV_, BlockColIdV_ + BlockColsV_ - 1)));
  }
  // Not lazy, see above.
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

template <wuzp_blitz::BlitzArray ImplementationT_>
struct Zero<AccessImplementation<ImplementationT_>> {
  static void run(ImplementationT_& impl) { impl = 0; }
};

template <wuzp_blitz::BlitzArray ImplementationT_>
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

template <wuzp_blitz::BlitzArray ImplementationT_>
struct Evaluate<AccessImplementation<ImplementationT_>> {
  // A new array, also for an array: the result of evaluate() does not share the values of the original.
  static auto value(const ImplementationT_& impl) { return wuzp_blitz::internal::copyToArray(impl); }
  using type = decltype(value(std::declval<const ImplementationT_>()));
};

}  // namespace wuzp
