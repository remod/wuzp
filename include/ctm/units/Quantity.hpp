#pragma once

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_CTM
#include "ctm/units/ctm/Quantity.hpp"
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "ctm/units/boost/Quantity.hpp"
#endif
