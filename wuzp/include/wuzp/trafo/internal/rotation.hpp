#pragma once

#include <stdexcept>
#include <utility>

#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/internal/validity.hpp"

namespace wuzp {
namespace internal {

// The validity check and the projection of the matrices of rotations, shared by ActiveSpatialRotation and
// PassiveSpatialRotation. The matrices are their values: rotations are dimensionless.

//! The determinant of a matrix in two dimensions, ad - bc.
template <typename ValuesT_>
  requires(Rows<ValuesT_>::value == 2)
auto rotationDeterminant(const ValuesT_& matrix) {
  return matrix.template get<0, 0>() * matrix.template get<1, 1>() - matrix.template get<0, 1>() * matrix.template get<1, 0>();
}

//! The determinant of a matrix in three dimensions, the triple product c0 . (c1 x c2) of its columns, computed with the cross and
//! dot products that every value matrix backend provides.
template <typename ValuesT_>
  requires(Rows<ValuesT_>::value == 3)
auto rotationDeterminant(const ValuesT_& matrix) {
  return matrix.template getBlock<0, 0, 3, 1>()
      .dotProduct(matrix.template getBlock<0, 1, 3, 1>().crossProduct(matrix.template getBlock<0, 2, 3, 1>()))
      .get();
}

//! The determinant of a matrix in other dimensions, which needs a value matrix backend with determinants
//! (InvertibleValueMatrixBackend).
template <typename ValuesT_>
  requires(Rows<ValuesT_>::value != 2 && Rows<ValuesT_>::value != 3)
auto rotationDeterminant(const ValuesT_& matrix) {
  return matrix.determinant();
}

//! Whether a square matrix is a rotation: max_ij |(R R^T - I)_ij| <= tolerance, and det(R) > 0, like Drake's
//! RotationMatrix::IsValid() (https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rotation_matrix.html). An orthogonal matrix
//! has the determinant 1 or -1, so a positive determinant rules out reflections. NaN elements make it invalid.
template <typename ValuesT_>
bool isRotation(const ValuesT_& matrix, const typename ValuesT_::ValueT tolerance) {
  return maxAbsDifference(ValuesT_{matrix.innerProduct(matrix.transpose())}, identity<ValuesT_>()) <= tolerance &&
         rotationDeterminant(matrix) > 0;
}

/**
 * The rotation nearest to a square matrix A, i.e. the orthogonal factor U of its polar decomposition A = U H, which minimizes
 * the Frobenius norm of A - U among all orthogonal matrices, computed by the Newton-Schulz iteration X <- X (3 I - X^T X) / 2,
 * starting with X = A. It needs only matrix products, so it works with every value matrix backend, unlike a singular value
 * decomposition. It converges quadratically to U if all singular values of A are in (0, sqrt(3)) (Higham, Functions of
 * Matrices, SIAM 2008, section 8.3), which ||I - A^T A|| < 1 guarantees in any consistent norm (Björck and Bowie, An iterative
 * algorithm for computing the best estimate of an orthogonal matrix, SIAM J. Numer. Anal. 8(2), 1971): the projection requires
 * this in the Frobenius norm, i.e. that A is close to orthogonal, e.g. a rotation whose rounding errors accumulated. It
 * iterates until the deviation of X^T X from the identity no longer decreases, at the level of rounding. The polar factor of a matrix with
 * a positive determinant is a rotation, otherwise a reflection.
 * @throws std::invalid_argument If ||I - A^T A|| is not smaller than 1 in the Frobenius norm, or if the determinant of A is not
 * positive, e.g. of a reflection.
 */
template <typename ValuesT_>
ValuesT_ projectToRotation(ValuesT_ matrix) {
  using ValueT = typename ValuesT_::ValueT;
  const auto identityMatrix = identity<ValuesT_>();
  ValuesT_ gram{matrix.transpose().innerProduct(matrix)};
  ValueT deviation = ValuesT_{gram - identityMatrix}.norm();
  // Also rejects NaN.
  if (!(deviation < ValueT{1})) {
    throw std::invalid_argument(
        "The matrix is too far from a rotation to be projected onto one: ||I - A^T A|| must be smaller than 1 in the Frobenius "
        "norm.");
  }
  // The deviation, the Frobenius norm of the eigenvalues d of X^T X - I, decreases strictly: each iteration maps d to
  // d^2 (d - 3) / 4, whose magnitude is smaller for |d| < 1. So the loop ends, at the latest when rounding stops the decrease.
  while (deviation > ValueT{0}) {
    ValuesT_ next{matrix.innerProduct(identityMatrix * ValueT{3} - gram) / ValueT{2}};
    ValuesT_ nextGram{next.transpose().innerProduct(next)};
    const ValueT nextDeviation = ValuesT_{nextGram - identityMatrix}.norm();
    if (!(nextDeviation < deviation)) {
      break;
    }
    matrix = std::move(next);
    gram = std::move(nextGram);
    deviation = nextDeviation;
  }
  if (!(rotationDeterminant(matrix) > ValueT{0})) {
    throw std::invalid_argument("The matrix cannot be projected onto a rotation: its determinant must be positive.");
  }
  return matrix;
}

}  // namespace internal
}  // namespace wuzp
