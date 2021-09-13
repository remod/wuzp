#pragma once

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp/units/wuzp/QuantityTraits.hpp"
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp/units/boost/QuantityTraits.hpp"
#endif
