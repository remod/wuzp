#pragma once

#if MATRIX_IMPLEMENTATION == MATRIX_IMPLEMENTATION_WUZP
#include "wuzp/value_matrix/wuzp/ValueMatrix.hpp"
#elif MATRIX_IMPLEMENTATION == MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp/value_matrix/eigen/ValueMatrix.hpp"
#endif
