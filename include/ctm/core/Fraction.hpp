#pragma once

namespace ctm {

template <int Dividend, int Divisor>
struct ReducedFraction {};

template <int Dividend, int Divisor, int Reduction, int DividendRemainder, int DivisorRemainder>
struct ReduceFractionImpl {
  static constexpr int NewReduction = Reduction - 1;
  using type = typename ReduceFractionImpl<Dividend, Divisor, NewReduction, Dividend % NewReduction, Divisor % NewReduction>::type;
};

template <int Dividend, int Divisor>
struct ReduceFractionImpl<Dividend, Divisor, 1, 0, 0> {
  using type = ReducedFraction<Dividend, Divisor>;
};

template <int Dividend, int Divisor, int Reduction>
struct ReduceFractionImpl<Dividend, Divisor, Reduction, 0, 0> {
  static constexpr int NewDividend = Dividend / Reduction;
  static constexpr int NewDivisor = Divisor / Reduction;
  static constexpr int NewReduction = NewDivisor;
  using type =
      typename ReduceFractionImpl<NewDividend, NewDivisor, NewReduction, NewDividend % NewReduction, NewDivisor % NewReduction>::type;
};

template <int Dividend, int Divisor>
struct ReduceFraction {
  static_assert(Divisor != 0, "The divisor must not be zero.");
  static constexpr int Sign = Divisor > 0 ? 1 : -1;
  // Always use a positive divisor.
  using type = typename ReduceFractionImpl<Dividend / Sign, Divisor / Sign, Divisor / Sign, Dividend % Divisor, 0>::type;
};

template <int Dividend, int Divisor>
using Fraction = typename ReduceFraction<Dividend, Divisor>::type;

}  // namespace ctm
