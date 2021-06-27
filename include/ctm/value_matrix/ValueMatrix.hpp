#pragma once

#if MATRIX_IMPLEMENTATION == MATRIX_IMPLEMENTATION_CTM
#include "ctm/value_matrix/ctm/ValueMatrix.hpp"
#elif MATRIX_IMPLEMENTATION == MATRIX_IMPLEMENTATION_EIGEN
#include "ctm/value_matrix/eigen/ValueMatrix.hpp"
#endif
