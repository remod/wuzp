#pragma once

// The value matrix backend of the wuzp benchmarks, chosen by the build like in the tests.

#if defined(WUZP_BENCHMARK_EIGEN)
#include <wuzp_eigen/ValueMatrix.hpp>
using wuzp_eigen::QuantityD;
using wuzp_eigen::ValueMatrix;
#elif defined(WUZP_BENCHMARK_BLITZ)
#include <wuzp_blitz/ValueMatrix.hpp>
using wuzp_blitz::QuantityD;
using wuzp_blitz::ValueMatrix;
#else
#error "Unknown value matrix backend: define WUZP_BENCHMARK_EIGEN or WUZP_BENCHMARK_BLITZ."
#endif
