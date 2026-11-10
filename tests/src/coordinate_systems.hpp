#include "wuzp/core/CoordinateSystem.hpp"

struct CoordinateSystemAId {};
struct CoordinateSystemBId {};
struct CoordinateSystemCId {};
struct CoordinateSystemDId {};
using CoordinateSystemA = wuzp::CoordinateSystem<CoordinateSystemAId>;
using CoordinateSystemB = wuzp::CoordinateSystem<CoordinateSystemBId>;
using CoordinateSystemC = wuzp::CoordinateSystem<CoordinateSystemCId>;
using CoordinateSystemD = wuzp::CoordinateSystem<CoordinateSystemDId>;

// Inertial frames of special relativity: coordinate systems with the Minkowski metric, in which spacetime vectors are expressed.
struct SpacetimeAId {};
struct SpacetimeBId {};
struct SpacetimeCId {};
struct SpacetimeDId {};
using SpacetimeA = wuzp::CoordinateSystem<SpacetimeAId, wuzp::Minkowski>;
using SpacetimeB = wuzp::CoordinateSystem<SpacetimeBId, wuzp::Minkowski>;
using SpacetimeC = wuzp::CoordinateSystem<SpacetimeCId, wuzp::Minkowski>;
using SpacetimeD = wuzp::CoordinateSystem<SpacetimeDId, wuzp::Minkowski>;
