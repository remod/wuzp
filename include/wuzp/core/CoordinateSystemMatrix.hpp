#pragma once

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/TypeMatrix.hpp"

namespace wuzp {

template <int Rows, int Cols, typename... CoordinateSystems>
using CoordinateSystemMatrix = TypeMatrix<Rows, Cols, CoordinateSystemTraits, CoordinateSystems...>;

}  // namespace wuzp
