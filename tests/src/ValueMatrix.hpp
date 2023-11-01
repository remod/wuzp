#pragma once

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
using namespace wuzp_eigen;  // NOLINT
#elif VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_BLITZ
#include "wuzp_blitz/ValueMatrix.hpp"
using namespace wuzp_blitz;  // NOLINT
#endif
