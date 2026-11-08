#pragma once

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_MP_UNITS
#include "wuzp_mp_units/Unit.hpp"
using namespace wuzp_mp_units;  // NOLINT
#else
#error "Unknown UNITS_IMPLEMENTATION"
#endif
