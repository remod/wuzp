#pragma once

#include "ctm/core/TypeMatrix.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_CTM
#include "ctm/unit_impl/ctm.hpp"
#include "ctm/unit_impl/UnitTraits.hpp"
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "ctm/boost/boost.hpp"
#endif

namespace ctm {

template <int Rows, int Cols, typename... Units>
using UnitMatrix = TypeMatrix<Rows, Cols, UnitTraits, Units...>;

}  // namespace ctm
