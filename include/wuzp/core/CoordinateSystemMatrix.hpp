#pragma once

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/TypeMatrix.hpp"

namespace wuzp {

template <int Rows, int Cols, typename... CoordinateSystems>
using CoordinateSystemMatrix = TypeMatrix<Rows, Cols, CoordinateSystemTraits, CoordinateSystems...>;

template <int Rows, int Cols, typename... CoordinateSystems, typename NewCoordinateSystem>
struct CastCoordinateSystem<TypeMatrixImpl<Rows, Cols, CoordinateSystemTraits, CoordinateSystems...>, NewCoordinateSystem> {
  static_assert(IsHomogeneous<CoordinateSystems...>::value, "Coordinate systems are heterogeneous.");
  using type = CoordinateSystemMatrix<Rows, Cols, NewCoordinateSystem>;
};

}  // namespace wuzp
