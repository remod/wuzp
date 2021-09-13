#pragma once

#if MATRIX_IMPLEMENTATION == MATRIX_IMPLEMENTATION_WUZP
#include "wuzp/value_matrix/wuzp/ValueMatrixTraits.hpp"
#elif MATRIX_IMPLEMENTATION == MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp/value_matrix/eigen/ValueMatrixTraits.hpp"
#endif
