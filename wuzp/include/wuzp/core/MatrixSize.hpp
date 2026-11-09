#pragma once

#include <algorithm>
#include <type_traits>
#include <utility>

#include "wuzp/core/traits.hpp"

namespace wuzp {

template <int... SizesV_>
struct MatrixSize {
  using SizesT = std::integer_sequence<int, SizesV_...>;
  static constexpr int NumElementsV = std::integral_constant<int, (SizesV_ * ...)>::value;
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
template <int RowsLhsV_, int ColsLhsV_>
struct Product<MatrixSize<RowsLhsV_, ColsLhsV_>, MatrixSize<1, 1>> {
  using type = MatrixSize<RowsLhsV_, ColsLhsV_>;
};
// Product of 1x1 matrix and matrix.
template <int RowsRhsV_, int ColsRhsV_>
struct Product<MatrixSize<1, 1>, MatrixSize<RowsRhsV_, ColsRhsV_>> {
  using type = typename Product<MatrixSize<RowsRhsV_, ColsRhsV_>, MatrixSize<1, 1>>::type;
};
// Product of matrix and matrix.
template <int RowsLhsV_, int ColsLhsV_, int RowsRhsV_, int ColsRhsV_>
struct Product<MatrixSize<RowsLhsV_, ColsLhsV_>, MatrixSize<RowsRhsV_, ColsRhsV_>> {
  static_assert(RowsLhsV_ == RowsRhsV_, "Number of rows do not match.");
  static_assert(ColsLhsV_ == ColsRhsV_, "Number of cols do not match.");
  using type = MatrixSize<RowsLhsV_, ColsLhsV_>;
};

template <int RowsLhsV_, int ColsLhsV_, int RowsRhsV_, int ColsRhsV_>
struct InnerProduct<MatrixSize<RowsLhsV_, ColsLhsV_>, MatrixSize<RowsRhsV_, ColsRhsV_>> {
  static_assert(ColsLhsV_ == RowsRhsV_, "Number of LHS cols does not match number of RHS rows.");
  using type = MatrixSize<RowsLhsV_, ColsRhsV_>;
};

template <int RowsLhsV_, int ColsLhsV_, int RowsRhsV_, int ColsRhsV_>
struct OuterProduct<MatrixSize<RowsLhsV_, ColsLhsV_>, MatrixSize<RowsRhsV_, ColsRhsV_>> {
  static_assert(std::min(RowsLhsV_, RowsRhsV_) <= 1,
                "Hypermatrices are not supported. At least one of the matrices must be a scalar or a vector.");
  static_assert(std::min(ColsLhsV_, ColsRhsV_) <= 1,
                "Hypermatrices are not supported. At least one of the matrices must be a scalar or a vector.");
  using type = MatrixSize<RowsLhsV_ + RowsRhsV_ - 1, ColsLhsV_ + ColsRhsV_ - 1>;
};

template <int RowsLhsV_, int ColsLhsV_, int RowsRhsV_, int ColsRhsV_>
struct DotProduct<MatrixSize<RowsLhsV_, ColsLhsV_>, MatrixSize<RowsRhsV_, ColsRhsV_>> {
  static_assert(RowsLhsV_ == RowsRhsV_, "Matrix sizes do not match.");
  static_assert(ColsLhsV_ == 1, "Dot product is only defined for vectors.");
  static_assert(ColsRhsV_ == 1, "Dot product is only defined for vectors.");
  using type = MatrixSize<1, 1>;
};

template <int RowsLhsV_, int ColsLhsV_, int RowsRhsV_, int ColsRhsV_>
struct CrossProduct<MatrixSize<RowsLhsV_, ColsLhsV_>, MatrixSize<RowsRhsV_, ColsRhsV_>> {
  static_assert(RowsLhsV_ == 3 && RowsRhsV_ == 3 && ColsLhsV_ == 1 && ColsRhsV_ == 1,
                "Cross product is only defined for three-dimensional vectors.");
  using type = MatrixSize<3, 1>;
};

template <int RowsLhsV_, int ColsLhsV_, int RowsRhsV_, int ColsRhsV_>
struct Division<MatrixSize<RowsLhsV_, ColsLhsV_>, MatrixSize<RowsRhsV_, ColsRhsV_>> {
  static_assert(RowsLhsV_ == RowsRhsV_, "Number of rows do not match.");
  static_assert(ColsLhsV_ == ColsRhsV_, "Number of cols do not match.");
  using type = MatrixSize<RowsLhsV_, ColsLhsV_>;
};
template <int RowsLhsV_, int ColsLhsV_>
struct Division<MatrixSize<RowsLhsV_, ColsLhsV_>, MatrixSize<1, 1>> {
  using type = MatrixSize<RowsLhsV_, ColsLhsV_>;
};

template <int RowsV_, int ColsV_>
struct Transpose<MatrixSize<RowsV_, ColsV_>> {
  using type = MatrixSize<ColsV_, RowsV_>;
};

//! The size of the matrix inverse. The elementwise reciprocal keeps the size of any matrix.
template <int RowsV_, int ColsV_>
struct Inverse<MatrixSize<RowsV_, ColsV_>> {
  static_assert(RowsV_ == ColsV_, "Matrix is not square.");
  using type = MatrixSize<RowsV_, ColsV_>;
};

template <int RowsV_, int ColsV_, int BlockRowIdV_, int BlockColIdV_, int BlockRowsV_, int BlockColsV_>
struct Block<MatrixSize<RowsV_, ColsV_>, BlockRowIdV_, BlockColIdV_, BlockRowsV_, BlockColsV_> {
  static_assert(BlockRowIdV_ >= 0 && BlockColIdV_ >= 0, "Block top left corner is out of range.");
  static_assert(BlockRowIdV_ + BlockRowsV_ <= RowsV_ && BlockColIdV_ + BlockColsV_ <= ColsV_, "Block bottom right corner is out of range.");
  using type = MatrixSize<BlockRowsV_, BlockColsV_>;
};

}  // namespace wuzp
