#pragma once

#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_WUZP
#include "wuzp/value_matrix/wuzp/ValueMatrix.hpp"
#elif VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp/value_matrix/eigen/ValueMatrix.hpp"
#endif
