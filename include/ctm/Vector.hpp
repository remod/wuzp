#pragma once

#include "ctm/QuantityMatrix.hpp"
#include "ctm/TypeMatrix.hpp"

namespace ctm {

template <int Dimension, typename Values, typename Frame, typename RelativeUnit>
using VectorTemplate = QuantityMatrix<Values, TypeMatrix<Dimension, 1, Frame>, TypeMatrix<Dimension, 1, RelativeUnit>>;

}  // namespace ctm
