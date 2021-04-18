#pragma once

#include "ctm/Matrix.hpp"
#include "ctm/TypeMatrix.hpp"

namespace ctm {

template <typename Values, typename Frame, typename Unit>
using TranslationTemplate = Matrix<Values, TypeMatrix<3, 1, Frame>, TypeMatrix<3, 1, Unit>>;

}
