#pragma once

#include "wuzp/core/CoordinateSystem.hpp"

struct CoordinateSystemAId {};
struct CoordinateSystemBId {};
struct CoordinateSystemCId {};
struct CoordinateSystemDId {};
using CoordinateSystemA = wuzp::CoordinateSystem<CoordinateSystemAId>;
using CoordinateSystemB = wuzp::CoordinateSystem<CoordinateSystemBId>;
using CoordinateSystemC = wuzp::CoordinateSystem<CoordinateSystemCId>;
using CoordinateSystemD = wuzp::CoordinateSystem<CoordinateSystemDId>;

// Points of rigid bodies, e.g. the points spatial vectors are about: a point is the origin of a coordinate system, e.g. of the
// coordinate system with the axes of the body (see SpatialVelocity).
struct PointPId {};
struct PointQId {};
using PointP = wuzp::CoordinateSystem<PointPId>;
using PointQ = wuzp::CoordinateSystem<PointQId>;

// Inertial frames of special relativity: coordinate systems with the Minkowski metric, in which spacetime vectors are expressed.
struct SpacetimeAId {};
struct SpacetimeBId {};
struct SpacetimeCId {};
struct SpacetimeDId {};
using SpacetimeA = wuzp::CoordinateSystem<SpacetimeAId, wuzp::Minkowski>;
using SpacetimeB = wuzp::CoordinateSystem<SpacetimeBId, wuzp::Minkowski>;
using SpacetimeC = wuzp::CoordinateSystem<SpacetimeCId, wuzp::Minkowski>;
using SpacetimeD = wuzp::CoordinateSystem<SpacetimeDId, wuzp::Minkowski>;
