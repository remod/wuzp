#pragma once

#include <numeric>

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

template <int Dividend, int Divisor>
struct ReduceRational {
  static_assert(Divisor != 0, "The divisor must not be zero.");

 private:
  // Always use a positive divisor, and divide by the greatest common divisor. Avoid dividing by zero in case of a zero divisor,
  // which is already reported above.
  static constexpr int Sign = Divisor > 0 ? 1 : -1;
  static constexpr int GreatestCommonDivisor = Divisor == 0 ? 1 : std::gcd(Dividend, Divisor);

 public:
  using type = ReducedRational<Sign * Dividend / GreatestCommonDivisor, Sign * Divisor / GreatestCommonDivisor>;
};

}  // namespace internal
}  // namespace wuzp
