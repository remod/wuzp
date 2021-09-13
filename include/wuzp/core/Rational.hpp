#pragma once

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {
namespace internal {

template <int Dividend, int Divisor>
struct ReducedRational {};

template <int Dividend, int Divisor, int Reduction, int DividendRemainder, int DivisorRemainder>
struct ReduceRationalImpl {
  static constexpr int NewReduction = Reduction - 1;
  using type = typename ReduceRationalImpl<Dividend, Divisor, NewReduction, Dividend % NewReduction, Divisor % NewReduction>::type;
};

template <int Dividend, int Divisor>
struct ReduceRationalImpl<Dividend, Divisor, 1, 0, 0> {
  using type = ReducedRational<Dividend, Divisor>;
};

template <int Dividend, int Divisor, int Reduction>
struct ReduceRationalImpl<Dividend, Divisor, Reduction, 0, 0> {
 private:
  static constexpr int NewDividend = Dividend / Reduction;
  static constexpr int NewDivisor = Divisor / Reduction;
  static constexpr int NewReduction = NewDivisor;

 public:
  using type =
      typename ReduceRationalImpl<NewDividend, NewDivisor, NewReduction, NewDividend % NewReduction, NewDivisor % NewReduction>::type;
};

template <int Dividend, int Divisor>
struct ReduceRational {
  static_assert(Divisor != 0, "The divisor must not be zero.");

 private:
  // Always use a positive divisor.
  static constexpr int Sign = Divisor > 0 ? 1 : -1;
  static constexpr int NewDividend = Dividend / Sign;
  static constexpr int NewDivisor = Divisor / Sign;
  static constexpr int NewReduction = NewDivisor;

 public:
  using type = typename ReduceRationalImpl<NewDividend, NewDivisor, NewReduction, Dividend % Divisor, 0>::type;
};

}  // namespace internal

template <int Dividend, int Divisor>
using Rational = typename internal::ReduceRational<Dividend, Divisor>::type;

struct RationalTraits {
  template <typename T>
  struct Identity;
  template <int Dividend, int Divisor>
  struct Identity<internal::ReducedRational<Dividend, Divisor>> {
    using type = Rational<1, 1>;
  };

  DEFINE_RECURSIVE_TRAIT(Sum)
  template <int DividendLhs, int DivisorLhs, int DividendRhs, int DivisorRhs>
  struct Sum<internal::ReducedRational<DividendLhs, DivisorLhs>, internal::ReducedRational<DividendRhs, DivisorRhs>> {
    using type = Rational<DividendLhs * DivisorRhs + DividendRhs * DivisorLhs, DivisorLhs * DivisorRhs>;
  };

  DEFINE_RECURSIVE_TRAIT(Difference)
  template <int DividendLhs, int DivisorLhs, int DividendRhs, int DivisorRhs>
  struct Difference<internal::ReducedRational<DividendLhs, DivisorLhs>, internal::ReducedRational<DividendRhs, DivisorRhs>> {
    using type = Rational<DividendLhs * DivisorRhs - DividendRhs * DivisorLhs, DivisorLhs * DivisorRhs>;
  };

  DEFINE_RECURSIVE_TRAIT(Product)
  template <int DividendLhs, int DivisorLhs, int DividendRhs, int DivisorRhs>
  struct Product<internal::ReducedRational<DividendLhs, DivisorLhs>, internal::ReducedRational<DividendRhs, DivisorRhs>> {
    using type = Rational<DividendLhs * DividendRhs, DivisorLhs * DivisorRhs>;
  };

  DEFINE_RECURSIVE_TRAIT(Division)
  template <int DividendLhs, int DivisorLhs, int DividendRhs, int DivisorRhs>
  struct Division<internal::ReducedRational<DividendLhs, DivisorLhs>, internal::ReducedRational<DividendRhs, DivisorRhs>> {
    using type = Rational<DividendLhs * DivisorRhs, DivisorLhs * DividendRhs>;
  };

  template <typename T>
  struct Inverse;
  template <int Dividend, int Divisor>
  struct Inverse<internal::ReducedRational<Dividend, Divisor>> {
    using type = Rational<Divisor, Dividend>;
  };
};

}  // namespace wuzp
