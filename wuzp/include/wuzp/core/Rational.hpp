#pragma once

#include "wuzp/core/internal/rational_operations.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

template <int DividendV_, int DivisorV_>
using Rational = typename internal::ReduceRational<DividendV_, DivisorV_>::type;

template <int DividendV_, int DivisorV_>
struct Identity<internal::ReducedRational<DividendV_, DivisorV_>> {
  using type = Rational<1, 1>;
};

template <int DividendLhsV_, int DivisorLhsV_, int DividendRhsV_, int DivisorRhsV_>
struct Sum<internal::ReducedRational<DividendLhsV_, DivisorLhsV_>, internal::ReducedRational<DividendRhsV_, DivisorRhsV_>> {
  using type = Rational<DividendLhsV_ * DivisorRhsV_ + DividendRhsV_ * DivisorLhsV_, DivisorLhsV_ * DivisorRhsV_>;
};

template <int DividendLhsV_, int DivisorLhsV_, int DividendRhsV_, int DivisorRhsV_>
struct Difference<internal::ReducedRational<DividendLhsV_, DivisorLhsV_>, internal::ReducedRational<DividendRhsV_, DivisorRhsV_>> {
  using type = Rational<DividendLhsV_ * DivisorRhsV_ - DividendRhsV_ * DivisorLhsV_, DivisorLhsV_ * DivisorRhsV_>;
};

template <int DividendV_, int DivisorV_>
struct Negation<internal::ReducedRational<DividendV_, DivisorV_>> {
  using type = Rational<-DividendV_, DivisorV_>;
};

template <int DividendLhsV_, int DivisorLhsV_, int DividendRhsV_, int DivisorRhsV_>
struct Product<internal::ReducedRational<DividendLhsV_, DivisorLhsV_>, internal::ReducedRational<DividendRhsV_, DivisorRhsV_>> {
  using type = Rational<DividendLhsV_ * DividendRhsV_, DivisorLhsV_ * DivisorRhsV_>;
};

template <int DividendLhsV_, int DivisorLhsV_, int DividendRhsV_, int DivisorRhsV_>
struct Division<internal::ReducedRational<DividendLhsV_, DivisorLhsV_>, internal::ReducedRational<DividendRhsV_, DivisorRhsV_>> {
  using type = Rational<DividendLhsV_ * DivisorRhsV_, DivisorLhsV_ * DividendRhsV_>;
};

// The result of an arbitrary rational powered by another arbitrary rational can be a real number which cannot be expressed as rational.
// Therefore, only powers of zero are defined, which are zero for every positive exponent.
template <int DividendExponentV_, int DivisorExponentV_>
struct Power<internal::ReducedRational<0, 1>, internal::ReducedRational<DividendExponentV_, DivisorExponentV_>> {
  static_assert(DividendExponentV_ > 0, "Zero raised to a non-positive power is not defined.");
  using type = Rational<0, 1>;
};

template <int DividendV_, int DivisorV_>
struct Inverse<internal::ReducedRational<DividendV_, DivisorV_>> {
  using type = Rational<DivisorV_, DividendV_>;
};

}  // namespace wuzp
