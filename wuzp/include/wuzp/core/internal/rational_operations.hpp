#pragma once

#include "wuzp/core/traits.hpp"

namespace wuzp {
namespace internal {

template <int DividendV, int DivisorV>
struct ReducedRational {
  static constexpr int Dividend = DividendV;
  static constexpr int Divisor = DivisorV;
  template <typename ValueT>
  static ValueT value() {
    return static_cast<ValueT>(Dividend) / static_cast<ValueT>(Divisor);
  }
};

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
}  // namespace wuzp
