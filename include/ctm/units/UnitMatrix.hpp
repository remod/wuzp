#pragma once

#include "ctm/core/TypeMatrix.hpp"
#include "ctm/units/UnitTraits.hpp"

namespace ctm {
namespace units {

template <int Rows, int Cols, typename... Units>
using UnitMatrix = TypeMatrix<Rows, Cols, UnitTraits, Units...>;

}
}  // namespace ctm
