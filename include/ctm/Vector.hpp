#pragma once

#include "ctm/FramedQuantityMatrix.hpp"
#include "ctm/TypeMatrix.hpp"

namespace ctm {

template <int Dimension, typename Values, typename Frame, typename RelativeUnit>
using VectorTemplate = FramedQuantityMatrix<Values, TypeMatrix<Dimension, 1, Frame>, TypeMatrix<Dimension, 1, RelativeUnit>>;

}  // namespace ctm
