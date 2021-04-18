#pragma once

#include "ctm/Matrix.hpp"
#include "ctm/TypeMatrix.hpp"

namespace ctm {

template <int Dimension, typename Values, typename Frame, typename Unit>
using TranslationTemplate = Matrix<Values, TypeMatrix<Dimension, 1, Frame>, TypeMatrix<Dimension, 1, Unit>>;

}
