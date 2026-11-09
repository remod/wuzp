#pragma once

#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveLinearTransformation.hpp"

namespace wuzp {

// TODO(remod): When instantiating a spatial rotation, test for determinant == 1.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionV_>
using ActiveSpatialRotation = ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_, DimensionV_,
                                                         typename AccessUnits<AccessImplementation<ValueT_>>::Dimensionless>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ActiveSpatialRotation2 = ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, 2>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ActiveSpatialRotation3 = ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, 3>;

}  // namespace wuzp
