#pragma once

#include "wuzp/trafo/PassiveProperRigidTransformationTemplate.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"
#include "wuzp/trafo/internal/intermediate_coordinate_system.hpp"

namespace wuzp {

//! The passive proper rigid transformation of a rotation and a spatial translation from From to To, which first rotates, then
//! translates: p -> R p + t (see PassiveProperRigidTransformationTemplate).
template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename FromT_, typename ToT_, typename ValueT_>
using PassiveProperRigidTransformation = PassiveProperRigidTransformationTemplate<
    FromT_, ToT_, PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, internal::Intermediate<FromT_, ToT_>, DimensionV_>,
    PassiveSpatialTranslation<ValuesT_, DimensionV_, ValueT_, internal::Intermediate<FromT_, ToT_>, ToT_>>;
template <template <typename, int, int> typename ValuesT_, typename FromT_, typename ToT_, typename ValueT_>
using PassiveProperRigidTransformation2 = PassiveProperRigidTransformation<ValuesT_, 2, FromT_, ToT_, ValueT_>;
template <template <typename, int, int> typename ValuesT_, typename FromT_, typename ToT_, typename ValueT_>
using PassiveProperRigidTransformation3 = PassiveProperRigidTransformation<ValuesT_, 3, FromT_, ToT_, ValueT_>;

}  // namespace wuzp
