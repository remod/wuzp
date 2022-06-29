#pragma once

#include <utility>

#include "wuzp/core/traits.hpp"

namespace wuzp {

template <int... SizesV_>
struct MatrixSize {
  using SizesT = std::integer_sequence<int, SizesV_...>;
};

template <int RowsV_, int ColsV_>
struct Rows<MatrixSize<RowsV_, ColsV_>> {
  static constexpr int value = RowsV_;
};

template <int RowsV_, int ColsV_>
struct Cols<MatrixSize<RowsV_, ColsV_>> {
  static constexpr int value = ColsV_;
};

template <int RowsLhsV_, int ColsLhsV_, int RowsRhsV_, int ColsRhsV_>
struct Sum<MatrixSize<RowsLhsV_, ColsLhsV_>, MatrixSize<RowsRhsV_, ColsRhsV_>> {
  static_assert(RowsLhsV_ == RowsRhsV_, "Number of rows do not match.");
  static_assert(ColsLhsV_ == ColsRhsV_, "Number of cols do not match.");
  using type = MatrixSize<RowsLhsV_, ColsLhsV_>;
};

template <int RowsLhsV_, int ColsLhsV_, int RowsRhsV_, int ColsRhsV_>
struct Difference<MatrixSize<RowsLhsV_, ColsLhsV_>, MatrixSize<RowsRhsV_, ColsRhsV_>> {
  static_assert(RowsLhsV_ == RowsRhsV_, "Number of rows do not match.");
  static_assert(ColsLhsV_ == ColsRhsV_, "Number of cols do not match.");
  using type = MatrixSize<RowsLhsV_, ColsLhsV_>;
};

// Product of 1x1 matrix and 1x1 matrix.
template <>
struct Product<MatrixSize<1, 1>, MatrixSize<1, 1>> {
  using type = MatrixSize<1, 1>;
};
// Product of matrix and 1x1 matrix.
template <int RowsLhs, int ColsLhs>
struct Product<MatrixSize<RowsLhs, ColsLhs>, MatrixSize<1, 1>> {
  using type = MatrixSize<RowsLhs, ColsLhs>;
};
// Product of 1x1 matrix and matrix.
template <int RowsRhs, int ColsRhs>
struct Product<MatrixSize<1, 1>, MatrixSize<RowsRhs, ColsRhs>> {
  using type = typename Product<MatrixSize<RowsRhs, ColsRhs>, MatrixSize<1, 1>>::type;
};
// Product of matrix and matrix.
template <int RowsLhs, int ColsLhs, int RowsRhs, int ColsRhs>
struct Product<MatrixSize<RowsLhs, ColsLhs>, MatrixSize<RowsRhs, ColsRhs>> {
  static_assert(ColsLhs == RowsRhs, "Number of LHS cols does not match number of RHS rows.");
  using type = MatrixSize<RowsLhs, ColsRhs>;
};

template <int RowsLhs, int ColsLhs>
struct Division<MatrixSize<RowsLhs, ColsLhs>, MatrixSize<1, 1>> {
  using type = MatrixSize<RowsLhs, ColsLhs>;
};

template <int RowsV_, int ColsV_>
struct Transpose<MatrixSize<RowsV_, ColsV_>> {
  using type = MatrixSize<ColsV_, RowsV_>;
};

template <int RowsLhs, int ColsLhs, int RowsRhs, int ColsRhs>
struct DotProduct<MatrixSize<RowsLhs, ColsLhs>, MatrixSize<RowsRhs, ColsRhs>> {
  static_assert(RowsLhs == RowsRhs, "Matrix sizes do not match.");
  static_assert(ColsLhs == 1, "Dot product is only defined for vectors.");
  static_assert(ColsRhs == 1, "Dot product is only defined for vectors.");
  using type = MatrixSize<1, 1>;
};

template <int RowsLhs, int ColsLhs, int RowsRhs, int ColsRhs>
struct CrossProduct<MatrixSize<RowsLhs, ColsLhs>, MatrixSize<RowsRhs, ColsRhs>> {
  static_assert(RowsLhs == 3 && RowsRhs == 3 && ColsLhs == 1 && ColsRhs == 1,
                "Cross product is only defined for three-dimensional vectors.");
  using type = MatrixSize<3, 1>;
};

template <int RowsV_, int ColsV_>
struct Inverse<MatrixSize<RowsV_, ColsV_>> {
  static_assert(RowsV_ == ColsV_, "Matrix is not square.");
  using type = MatrixSize<RowsV_, ColsV_>;
};

template <int RowsV_, int ColsV_, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
struct Block<MatrixSize<RowsV_, ColsV_>, BlockRowId, BlockColId, BlockRows, BlockCols> {
  using type = MatrixSize<BlockRows, BlockCols>;
};

}  // namespace wuzp
