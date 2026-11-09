#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Unit.hpp"
#include "wuzp/core/traits.hpp"

// The units of the matrix inverse and of the determinant. Both are only defined for matrices whose units describe a linear map,
// M_ij = a_i / b_j for some units a_i and b_j, i.e. whose unit matrix is the outer product of a column of units and a row of
// inverse units (Hart, Multidimensional Analysis, 1995, sections 3.1 and 3.3, https://doi.org/10.1007/978-1-4612-4208-6).
// E.g. a mass matrix maps accelerations to forces and torques, so its units are
// [kg, kg m; kg m, kg m^2] = [N, N m]^T [1 / (m / s^2), 1 / (1 / s^2)].

namespace wuzp {

namespace internal {

template <typename UnitsT_, typename IndexSequenceT_>
struct UnitsDescribeLinearMapImpl;
template <typename UnitsT_, std::size_t... Is>
struct UnitsDescribeLinearMapImpl<UnitsT_, std::index_sequence<Is...>> {
 private:
  static constexpr int ColsV = Cols<UnitsT_>::value;
  template <int RowIdV_, int ColIdV_>
  using ElementT = typename Element<UnitsT_, RowIdV_, ColIdV_>::type;
  template <int RowIdV_, int ColIdV_>
  static constexpr bool IsLinearV = std::is_same_v<typename Product<ElementT<RowIdV_, ColIdV_>, ElementT<0, 0>>::type,
                                                   typename Product<ElementT<RowIdV_, 0>, ElementT<0, ColIdV_>>::type>;

 public:
  static constexpr bool value = (IsLinearV<static_cast<int>(Is) / ColsV, static_cast<int>(Is) % ColsV> && ...);
};

}  // namespace internal

/**
 * Whether the units of a matrix describe a linear map, M_ij = a_i / b_j (Hart, section 3.1, see above): then M_ij M_00
 * = a_i a_0 / (b_j b_0) = M_i0 M_0j for all i and j, and conversely, a_i = M_i0 and b_j = M_00 / M_0j satisfy M_ij = a_i / b_j
 * if M_ij M_00 = M_i0 M_0j. Only for these matrices, every summand of the elements of a matrix product has the same unit, e.g.
 * of M M^-1, and every term of the determinant.
 */
template <typename UnitsT_>
struct UnitsDescribeLinearMap {
  static constexpr bool value =
      internal::UnitsDescribeLinearMapImpl<UnitsT_,
                                           std::make_index_sequence<std::size_t{Rows<UnitsT_>::value * Cols<UnitsT_>::value}>>::value;
};

namespace internal {

//! Rejects the units of a matrix that is not square or does not describe a linear map, for the matrix inverse and the
//! determinant: `static_assert(RequireSquareLinearMap<UnitsT_>::value)`. The value is always true, so that each failure reports
//! one message: the second check assumes that the first one passes.
template <typename UnitsT_>
struct RequireSquareLinearMap {
  static_assert(Rows<UnitsT_>::value == Cols<UnitsT_>::value, "Matrix is not square.");
  static_assert(Rows<UnitsT_>::value != Cols<UnitsT_>::value || UnitsDescribeLinearMap<UnitsT_>::value,
                "The units of the matrix do not describe a linear map (M_ij = a_i / b_j).");
  static constexpr bool value = true;
};

}  // namespace internal

/**
 * The units of the matrix inverse: (M^-1)_ij = 1 / M_ji = b_i / a_j for M_ij = a_i / b_j, i.e. the transpose of the reciprocals.
 * The inverse maps the units a_j back to b_i (Hart, section 3.3, see above):
 *
 *   q = M p:       [a]   [a/x a/y]   [x]        p = M^-1 q:    [x]   [x/a x/b]   [a]
 *                  [b] = [b/x b/y] * [y]                       [y] = [y/a y/b] * [b]
 *
 * E.g. the inverse of a mass matrix [kg, kg m; kg m, kg m^2] has the units [1/kg, 1/(kg m); 1/(kg m), 1/(kg m^2)]. Whether the
 * values are invertible is only known at run time.
 */
template <int RowsV_, int ColsV_, typename... UnitImplementationsT_>
struct Inverse<TypeMatrixImpl<RowsV_, ColsV_, std::tuple<Unit<UnitImplementationsT_>...>>> {
 private:
  using UnitsT = TypeMatrixImpl<RowsV_, ColsV_, std::tuple<Unit<UnitImplementationsT_>...>>;

 public:
  static_assert(internal::RequireSquareLinearMap<UnitsT>::value);
  using type = typename Transpose<typename CwiseInverse<UnitsT>::type>::type;
};

template <typename Matrix, typename IndexSequence>
struct DiagonalProductImpl;
template <typename Matrix, size_t... Is>
struct DiagonalProductImpl<Matrix, std::index_sequence<Is...>> {
  using type = typename Product<typename Element<Matrix, static_cast<int>(Is), static_cast<int>(Is)>::type...>::type;
};

/**
 * The unit of the determinant of a matrix.
 *
 * Only matrices whose units describe a linear map (M_ij = a_i / b_j, see UnitsDescribeLinearMap) have a determinant with a
 * well-defined unit: every term of the determinant then has the unit (a_0 * ... * a_n-1) / (b_0 * ... * b_n-1), which is the
 * product of the units on the diagonal. For a matrix with a single unit u, this is u^n.
 */
template <int RowsV_, int ColsV_, typename... UnitImplementationsT_>
struct Determinant<TypeMatrixImpl<RowsV_, ColsV_, std::tuple<Unit<UnitImplementationsT_>...>>> {
 private:
  using UnitsT = TypeMatrixImpl<RowsV_, ColsV_, std::tuple<Unit<UnitImplementationsT_>...>>;

 public:
  static_assert(internal::RequireSquareLinearMap<UnitsT>::value);
  using type = typename DiagonalProductImpl<UnitsT, std::make_index_sequence<std::size_t{RowsV_}>>::type;
};

}  // namespace wuzp
