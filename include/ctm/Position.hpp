#pragma once

#include "ctm/core/FramedQuantityMatrix.hpp"
#include "ctm/core/TypeMatrix.hpp"

namespace ctm {

template <int Dimension, typename Values, typename Frame, typename AbsoluteUnit>
using PositionTemplate = FramedQuantityMatrix<Values, TypeMatrix<Dimension, 1, Frame>, TypeMatrix<Dimension, 1, AbsoluteUnit>>;

}  // namespace ctm
