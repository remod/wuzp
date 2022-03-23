#pragma once

#include "wuzp/trafo/PassiveLinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units>
using Jacobian = PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>;

}  // namespace wuzp
