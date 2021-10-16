#pragma once

#include "wuzp/trafo/LinearTransformation.hpp"

namespace wuzp {

template <typename Scalar, typename CoordinateSystem, typename From, typename To, int DimensionFrom, int DimensionTo, typename... Units>
using Jacobian = LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>;

}  // namespace wuzp
