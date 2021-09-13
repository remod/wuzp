#pragma once

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp/units/wuzp/UnitTraits.hpp"
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp/units/boost/UnitTraits.hpp"
#endif
