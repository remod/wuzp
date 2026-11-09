#pragma once

#include <numeric>

#include "wuzp/core/traits.hpp"

namespace wuzp {
namespace internal {

template <int DividendV_, int DivisorV_>
struct ReducedRational {
  static constexpr int Dividend = DividendV_;
  static constexpr int Divisor = DivisorV_;
  template <typename ValueT_>
  static ValueT_ value() {
    return static_cast<ValueT_>(Dividend) / static_cast<ValueT_>(Divisor);
  }
};

template <int DividendV_, int DivisorV_>
struct ReduceRational {
  static_assert(DivisorV_ != 0, "The divisor must not be zero.");

 private:
  // Always use a positive divisor, and divide by the greatest common divisor. Avoid dividing by zero in case of a zero divisor,
  // which is already reported above.
  static constexpr int Sign = DivisorV_ > 0 ? 1 : -1;
  static constexpr int GreatestCommonDivisor = DivisorV_ == 0 ? 1 : std::gcd(DividendV_, DivisorV_);

 public:
  using type = ReducedRational<Sign * DividendV_ / GreatestCommonDivisor, Sign * DivisorV_ / GreatestCommonDivisor>;
};

}  // namespace internal
}  // namespace wuzp
