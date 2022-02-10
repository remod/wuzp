#pragma once

#include "wuzp/core/CoordinateSystemTemplate.hpp"
#include "wuzp/core/TypeMatrix.hpp"

namespace wuzp {

// template <int Rows, int Cols, typename... CoordinateSystems>
// using CoordinateSystemMatrix = TypeMatrix<Rows, Cols, CoordinateSystems...>;

template <int Rows, int Cols, typename... CoordinateSystemNames>
struct Inverse<TypeMatrixImpl<Rows, Cols, std::tuple<CoordinateSystemTemplate<CoordinateSystemNames>...>>> {
  static_assert(Rows == Cols, "Matrix is not square.");
  // We only allow homogeneous coordinate system matrices to be inverted.
  static_assert(IsHomogeneous<CoordinateSystemNames...>::value, "Coordinate systems are heterogeneous.");
  using type = TypeMatrixImpl<Rows, Cols, std::tuple<CoordinateSystemTemplate<CoordinateSystemNames>...>>;
};

template <int Rows, int Cols, typename... CoordinateSystemNames, typename NewCoordinateSystem>
struct CastCoordinateSystem<TypeMatrixImpl<Rows, Cols, std::tuple<CoordinateSystemTemplate<CoordinateSystemNames>...>>,
                            NewCoordinateSystem> {
  static_assert(IsHomogeneous<CoordinateSystemNames...>::value, "Coordinate systems are heterogeneous.");
  using type = TypeMatrix<Rows, Cols, NewCoordinateSystem>;
};

}  // namespace wuzp
