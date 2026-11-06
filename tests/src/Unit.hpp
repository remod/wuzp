#pragma once

// ANGLES_ARE_DIMENSIONLESS: wuzp's own units treat angles as dimensionless, Boost.Units as a dimension of its own and
// mp-units as a unit of its own. There, e.g. an angular velocity times a length is not a velocity, and the tests divide
// such results by a radian (see the TODO about angles in DESIGN.md).
#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#define ANGLES_ARE_DIMENSIONLESS 1
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#define ANGLES_ARE_DIMENSIONLESS 0
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_MP_UNITS
#include "wuzp_mp_units/Unit.hpp"
using namespace wuzp_mp_units;  // NOLINT
#define ANGLES_ARE_DIMENSIONLESS 0
#else
#error "Unknown UNITS_IMPLEMENTATION"
#endif
