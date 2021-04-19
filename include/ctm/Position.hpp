#pragma once

#include "ctm/QuantityMatrix.hpp"
#include "ctm/TypeMatrix.hpp"

namespace ctm {

template <int Dimension, typename Values, typename Frame, typename Unit>
using PositionTemplate = QuantityMatrix<Values, TypeMatrix<Dimension, 1, Frame>, TypeMatrix<Dimension, 1, Unit>>;

}  // namespace ctm
