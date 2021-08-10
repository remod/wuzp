#pragma once

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_CTM
#include "ctm/units/ctm/QuantityTraits.hpp"
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "ctm/units/boost/QuantityTraits.hpp"
#endif
