#pragma once

#include "wuzp/trafo/ActiveProperRigidTransformationTemplate.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"

namespace wuzp {

//! The active proper rigid transformation of a rotation and a spatial translation, which first rotates, then translates:
//! x -> R x + t (see ActiveProperRigidTransformationTemplate).
template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename CoordinateSystemT_, typename ValueT_>
using ActiveProperRigidTransformation =
    ActiveProperRigidTransformationTemplate<CoordinateSystemT_, ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>,
                                            ActiveSpatialTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_>>;
template <template <typename, int, int> typename ValuesT_, typename CoordinateSystemT_, typename ValueT_>
using ActiveProperRigidTransformation2 = ActiveProperRigidTransformation<ValuesT_, 2, CoordinateSystemT_, ValueT_>;
template <template <typename, int, int> typename ValuesT_, typename CoordinateSystemT_, typename ValueT_>
using ActiveProperRigidTransformation3 = ActiveProperRigidTransformation<ValuesT_, 3, CoordinateSystemT_, ValueT_>;

}  // namespace wuzp
