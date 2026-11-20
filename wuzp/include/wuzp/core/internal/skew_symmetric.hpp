#pragma once

#include <tuple>

// The formulas of the skew-symmetric matrix of a three-dimensional vector and of its inverse on the elements, shared by
// skewSymmetricMatrix() and vectorOfSkewSymmetricMatrix() of tensors and by the derivatives of rotations, which compute on the
// values of their matrices (see wuzp/trafo/internal/rotation_parameterizations.hpp).

namespace wuzp::internal {

//! The skew-symmetric matrix [v]x of the vector v = (x, y, z), the matrix of the cross product with v, [v]x w = v x w, as MatrixT_,
//! e.g. values or a tensor, constructed from its nine elements, row by row.
template <typename MatrixT_, typename ValueT_>
MatrixT_ skewSymmetric(const ValueT_ x, const ValueT_ y, const ValueT_ z) {
  return MatrixT_{ValueT_{0}, -z, y, z, ValueT_{0}, -x, -y, x, ValueT_{0}};
}

//! The elements (M_21 - M_12, M_02 - M_20, M_10 - M_01) / 2 of the vector of the skew-symmetric part (M - M^T) / 2 of a 3x3 matrix
//! M, the inverse of skewSymmetric() for a skew-symmetric matrix, which averages out the rounding errors of a matrix that is
//! skew-symmetric only approximately, e.g. dR/dt R^T. The matrix is values, whose elements are values, or quantities, whose
//! elements are quantities, whose differences check that both elements of each pair have the same unit; the elements of the
//! vector may have different units, so they are a std::tuple.
template <typename MatrixT_>
auto vectorOfSkewSymmetricPart(const MatrixT_& matrix) {
  using ValueT = typename MatrixT_::ValueT;
  return std::tuple{(matrix.template get<2, 1>() - matrix.template get<1, 2>()) / ValueT{2},
                    (matrix.template get<0, 2>() - matrix.template get<2, 0>()) / ValueT{2},
                    (matrix.template get<1, 0>() - matrix.template get<0, 1>()) / ValueT{2}};
}

}  // namespace wuzp::internal
