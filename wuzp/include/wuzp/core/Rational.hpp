#pragma once

#include "wuzp/core/internal/rational_operations.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

template <int Dividend, int Divisor>
using Rational = typename internal::ReduceRational<Dividend, Divisor>::type;

template <int Dividend, int Divisor>
struct Identity<internal::ReducedRational<Dividend, Divisor>> {
  using type = Rational<1, 1>;
};

template <int DividendLhs, int DivisorLhs, int DividendRhs, int DivisorRhs>
struct Sum<internal::ReducedRational<DividendLhs, DivisorLhs>, internal::ReducedRational<DividendRhs, DivisorRhs>> {
  using type = Rational<DividendLhs * DivisorRhs + DividendRhs * DivisorLhs, DivisorLhs * DivisorRhs>;
};

template <int DividendLhs, int DivisorLhs, int DividendRhs, int DivisorRhs>
struct Difference<internal::ReducedRational<DividendLhs, DivisorLhs>, internal::ReducedRational<DividendRhs, DivisorRhs>> {
  using type = Rational<DividendLhs * DivisorRhs - DividendRhs * DivisorLhs, DivisorLhs * DivisorRhs>;
};

template <int Dividend, int Divisor>
struct Negation<internal::ReducedRational<Dividend, Divisor>> {
  using type = Rational<-Dividend, Divisor>;
};

template <int DividendLhs, int DivisorLhs, int DividendRhs, int DivisorRhs>
struct Product<internal::ReducedRational<DividendLhs, DivisorLhs>, internal::ReducedRational<DividendRhs, DivisorRhs>> {
  using type = Rational<DividendLhs * DividendRhs, DivisorLhs * DivisorRhs>;
};

template <int DividendLhs, int DivisorLhs, int DividendRhs, int DivisorRhs>
struct Division<internal::ReducedRational<DividendLhs, DivisorLhs>, internal::ReducedRational<DividendRhs, DivisorRhs>> {
  using type = Rational<DividendLhs * DivisorRhs, DivisorLhs * DividendRhs>;
};

// The result of an arbitrary rational powered by another arbitrary rational can be a real number which cannot be expressed as rational.
// Therefore, only powers of zero are defined, which are zero for every positive exponent.
template <int DividendExponent, int DivisorExponent>
struct Power<internal::ReducedRational<0, 1>, internal::ReducedRational<DividendExponent, DivisorExponent>> {
  static_assert(DividendExponent > 0, "Zero raised to a non-positive power is not defined.");
  using type = Rational<0, 1>;
};

template <int Dividend, int Divisor>
struct Inverse<internal::ReducedRational<Dividend, Divisor>> {
  using type = Rational<Divisor, Dividend>;
};

}  // namespace wuzp
