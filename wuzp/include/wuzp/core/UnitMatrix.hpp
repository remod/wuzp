#pragma once

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Unit.hpp"

namespace wuzp {

// TODO(remod): Do we still need this?
// template <int Rows, int Cols, typename... Implementations>
// using UnitMatrix = TypeMatrix<Rows, Cols, Unit<Implementations>...>;

template <int Rows, int Cols, typename... UnitImplementations>
struct Inverse<TypeMatrixImpl<Rows, Cols, std::tuple<Unit<UnitImplementations>...>>> {
  static_assert(Rows == Cols, "Matrix is not square.");
  // Invert the types individually and transpose to get a candidate for the result of the inversion.
  using type = typename Transpose<TypeMatrixImpl<Rows, Cols, std::tuple<typename Inverse<Unit<UnitImplementations>>::type...>>>::type;

 private:
  // Verify the candidate: If the candidate multiplied by itself is idempotent (A^2 == A), it is valid.
  using Identity = typename Product<TypeMatrixImpl<Rows, Cols, std::tuple<Unit<UnitImplementations>...>>, type>::type;
  static_assert(std::is_same<typename Product<Identity, Identity>::type, Identity>::value, "Matrix is not invertible.");

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

template <int Rows, int Cols, typename... UnitImplementations>
struct IsAbsolute<TypeMatrixImpl<Rows, Cols, std::tuple<Unit<UnitImplementations>...>>> {
  static_assert(IsHomogeneous<std::integral_constant<bool, IsAbsolute<Unit<UnitImplementations>>::value>...>::value,
                "Unit matrix is mixed absolute/relative.");
  static constexpr bool value = (IsAbsolute<Unit<UnitImplementations>>::value && ...);
};
template <int Rows, int Cols, typename... UnitImplementations>
struct MakeRelative<TypeMatrixImpl<Rows, Cols, std::tuple<Unit<UnitImplementations>...>>> {
  using type = TypeMatrix<Rows, Cols, typename MakeRelative<Unit<UnitImplementations>>::type...>;
};
template <int Rows, int Cols, typename... UnitImplementations>
struct MakeAbsolute<TypeMatrixImpl<Rows, Cols, std::tuple<Unit<UnitImplementations>...>>> {
  using type = TypeMatrix<Rows, Cols, typename MakeAbsolute<Unit<UnitImplementations>>::type...>;
};

}  // namespace wuzp
