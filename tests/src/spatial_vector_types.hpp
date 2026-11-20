#pragma once

// Spatial vectors with double values, and their parts in the coordinate systems A and B, for the unit tests and the compile tests
// of the spatial vectors (#76).

#include "wuzp/core/Tensor.hpp"
#include "wuzp/trafo/SpatialAcceleration.hpp"
#include "wuzp/trafo/SpatialForce.hpp"
#include "wuzp/trafo/SpatialVelocity.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

template <typename MeasuredInT_, typename FrameT_, typename PointT_, typename ExpressedInT_>
using SpatialVelocityD = wuzp::SpatialVelocity<ValueMatrix, double, MeasuredInT_, FrameT_, PointT_, ExpressedInT_>;
template <typename MeasuredInT_, typename FrameT_, typename PointT_, typename ExpressedInT_>
using SpatialAccelerationD = wuzp::SpatialAcceleration<ValueMatrix, double, MeasuredInT_, FrameT_, PointT_, ExpressedInT_>;
template <typename FrameT_, typename PointT_, typename ExpressedInT_>
using SpatialForceD = wuzp::SpatialForce<ValueMatrix, double, FrameT_, PointT_, ExpressedInT_>;

using PositionInA = wuzp::PositionVector3<ValueMatrix, double, CoordinateSystemA>;
using VelocityInA = wuzp::VelocityVector3<ValueMatrix, double, CoordinateSystemA>;
using VelocityInB = wuzp::VelocityVector3<ValueMatrix, double, CoordinateSystemB>;
using AngularVelocityInA = wuzp::AngularVelocityVector3<ValueMatrix, double, CoordinateSystemA>;
using AngularVelocityInB = wuzp::AngularVelocityVector3<ValueMatrix, double, CoordinateSystemB>;
using AccelerationInA = wuzp::AccelerationVector3<ValueMatrix, double, CoordinateSystemA>;
using AccelerationInB = wuzp::AccelerationVector3<ValueMatrix, double, CoordinateSystemB>;
using AngularAccelerationInA = wuzp::AngularAccelerationVector3<ValueMatrix, double, CoordinateSystemA>;
using AngularAccelerationInB = wuzp::AngularAccelerationVector3<ValueMatrix, double, CoordinateSystemB>;
using ForceInA = wuzp::ForceVector3<ValueMatrix, double, CoordinateSystemA>;
using ForceInB = wuzp::ForceVector3<ValueMatrix, double, CoordinateSystemB>;
using TorqueInA = wuzp::TorqueVector3<ValueMatrix, double, CoordinateSystemA>;
using TorqueInB = wuzp::TorqueVector3<ValueMatrix, double, CoordinateSystemB>;

//! Whether the spatial vector shifts to the point Q by the offset, and for a spatial acceleration the spatial velocity of its frame,
//! e.g. not by quantities, which have no coordinate system and are neither a point nor a vector.
template <typename SpatialVectorT_, typename OffsetT_, typename... VelocityT_>
concept ShiftsToQBy = requires(const SpatialVectorT_& spatialVector, const OffsetT_& offset, const VelocityT_&... velocity) {
  spatialVector.template shift<PointQ>(offset, velocity...);
};
