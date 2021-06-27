#pragma once

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_CTM
#include "ctm/units/ctm/UnitTraits.hpp"
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "ctm/units/boost/UnitTraits.hpp"
#endif
