#pragma once

#include "ctm/core/FramedQuantityMatrix.hpp"
#include "ctm/core/TypeMatrix.hpp"

namespace ctm {

// TODO(remod): Does an absolute velocity exist?
template <int Dimension, typename Values, typename Frame, typename RelativeUnit>
using VelocityTemplate = FramedQuantityMatrix<Values, TypeMatrix<Dimension, 1, Frame>, TypeMatrix<Dimension, 1, RelativeUnit>>;

}  // namespace ctm
