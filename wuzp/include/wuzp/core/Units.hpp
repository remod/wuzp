#pragma once

#include <cstddef>
#include <tuple>
#include <utility>

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Unit.hpp"

namespace wuzp {

template <int Rows, int Cols, typename... UnitImplementations>
struct TensorInverse<TypeMatrixImpl<Rows, Cols, std::tuple<Unit<UnitImplementations>...>>> {
  static_assert(Rows == Cols, "Matrix is not square.");
  // Invert the types individually and transpose to get a candidate for the result of the inversion.
  using type = typename Transpose<TypeMatrixImpl<Rows, Cols, std::tuple<typename Inverse<Unit<UnitImplementations>>::type...>>>::type;

 private:
  // Verify the candidate: If the candidate contracted by itself is idempotent (A^2 == A), it is valid.
  using Identity = typename InnerProduct<TypeMatrixImpl<Rows, Cols, std::tuple<Unit<UnitImplementations>...>>, type>::type;
  static_assert(std::is_same<typename InnerProduct<Identity, Identity>::type, Identity>::value, "Matrix is not invertible.");

  /* Proof:

     Unit analysis of a linear transformation (q = M * p).
     a   a/x a/y a/z   x
     b = b/x b/y b/z * y
     c   c/x c/y c/z   z

     Unit analysis of the inverted linear transformation (p = M^-1 * q).
     One can see that the units are inverted and then transformed.
     x   x/a x/b x/c   a
     y = y/a y/b y/c * b
     z   z/a z/b z/c   c

     Unit analysis of the product M * M^1.
     One can see that the resulting identity matrix is asymmetric (!) but idempotent.
     1    a/b  a/c
     b/a  1    b/c
     c/a  c/b  1

     Example: 2D mass matrix, with one linear and one angular dimension.

     force = mass matrix * acceleration
     ml/tt  = m  ml  * l/tt
     mll/tt = ml mll * 1/tt

     inverted mass matrix * mass matrix = identity
     1/m  1/ml  * m  ml  = 1   l
     1/ml 1/mll * ml mll = 1/l 1
  */
};

template <typename Matrix, typename IndexSequence>
struct DiagonalProduct;
template <typename Matrix, size_t... Is>
struct DiagonalProduct<Matrix, std::index_sequence<Is...>> {
  using type = typename Product<typename Element<Matrix, static_cast<int>(Is), static_cast<int>(Is)>::type...>::type;
};

/**
 * The unit of the determinant of a matrix.
 *
 * Only matrices whose units describe a linear map (M_ij = a_i / b_j) have a determinant with a well-defined unit: every
 * term of the determinant then has the unit (a_0 * ... * a_n-1) / (b_0 * ... * b_n-1), which is the product of the units
 * on the diagonal. For a matrix with a single unit u, this is u^n.
 */
template <int Rows, int Cols, typename... UnitImplementations>
struct Determinant<TypeMatrixImpl<Rows, Cols, std::tuple<Unit<UnitImplementations>...>>> {
  static_assert(Rows == Cols, "Matrix is not square.");

 private:
  using MatrixT = TypeMatrixImpl<Rows, Cols, std::tuple<Unit<UnitImplementations>...>>;
  // Inverting the units checks that they describe a linear map.
  using InverseT = typename TensorInverse<MatrixT>::type;

 public:
  using type = typename DiagonalProduct<MatrixT, std::make_index_sequence<Rows>>::type;
};

}  // namespace wuzp
