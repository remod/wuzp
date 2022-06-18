#pragma once

#include "wuzp/trafo/ActiveLinearTransformation.hpp"

namespace wuzp {

// TODO(remod): When instantiating a spacial rotation, test for determinant == 1.
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, int Dimension>
using ActiveSpatialRotation = ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, Dimension, Dimension,
                                                         typename AccessUnits<SpecializationDummy<Scalar>>::Dimensionless>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using ActiveSpatialRotation2 = ActiveSpatialRotation<ValuesT_, Scalar, CoordinateSystem, 2>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem>
using ActiveSpatialRotation3 = ActiveSpatialRotation<ValuesT_, Scalar, CoordinateSystem, 3>;

}  // namespace wuzp
