#pragma once

#include "wuzp/trafo/PassiveLinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_>
using Jacobian =
    PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>;

}  // namespace wuzp
