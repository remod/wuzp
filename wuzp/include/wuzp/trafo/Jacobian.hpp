#pragma once

#include "wuzp/trafo/PassiveLinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units>
using Jacobian = PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>;

}  // namespace wuzp
