#pragma once

#include "wuzp/trafo/ActiveLinearTransformation.hpp"

namespace wuzp {

// TODO(remod): When instantiating a spatial rotation, test for determinant == 1.
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, int Dimension>
using ActiveSpatialRotation = ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, Dimension, Dimension,
                                                         typename AccessUnits<AccessImplementation<Value>>::Dimensionless>;
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem>
using ActiveSpatialRotation2 = ActiveSpatialRotation<ValuesT_, Value, CoordinateSystem, 2>;
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem>
using ActiveSpatialRotation3 = ActiveSpatialRotation<ValuesT_, Value, CoordinateSystem, 3>;

}  // namespace wuzp
