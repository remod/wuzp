#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>

#include "wuzp/core/Rational.hpp"

// The real powers and the hypotenuses of values, which pow(), sqrt(), cbrt() and hypot() of quantities apply to their values (see
// Quantities.hpp). Those of integer values are truncated exactly (see "Integer values" in DESIGN.md): their floating point values
// only estimate them, e.g. 8^(2/3) is 3.9999999999999996 with std::pow, since the exponent 2/3 is not exact, and sqrt(10^14 - 1)
// is within 5e-15 of 10^7, relative. The estimates are corrected with exact powers of 64-bit integers.

namespace wuzp::internal {

//! Throws the std::range_error of an integer result that does not fit into its value type: converting it would be undefined
//! behavior.
[[noreturn]] inline void throwOutOfRange() {
  throw std::range_error("The result does not fit into the integer value type.");
}

//! The floating point estimate of a non-negative integer result, truncated: of a power or a hypotenuse that is not computed
//! exactly, since x^P or x^2 + y^2 does not fit into 64 bits.
//! @throws std::range_error If the estimate is at least 2^64, above which it does not fit into any integer value type.
inline std::uintmax_t truncateEstimate(const double estimate) {
  // 2^64.
  constexpr double TwoToThe64V = 18446744073709551616.0;
  if (!(estimate < TwoToThe64V)) {
    throwOutOfRange();
  }
  return static_cast<std::uintmax_t>(estimate);
}

//! The magnitude |value| of an integer, as an unsigned integer, also of the minimum of a signed type, e.g. INT_MIN = -2^31,
//! whose magnitude the type cannot represent.
template <typename ValueT_>
std::uintmax_t magnitudeOf(const ValueT_ value) {
  if constexpr (std::is_signed_v<ValueT_>) {
    if (value < ValueT_{0}) {
      return static_cast<std::uintmax_t>(-(value + ValueT_{1})) + 1U;
    }
  }
  return static_cast<std::uintmax_t>(value);
}

//! The integer of the sign and the magnitude in the value type ValueT_.
//! @throws std::range_error If the value type cannot represent it, e.g. 2^31 as an int.
template <typename ValueT_>
ValueT_ fromMagnitude(const bool negative, const std::uintmax_t magnitude) {
  const auto maximum = static_cast<std::uintmax_t>(std::numeric_limits<ValueT_>::max());
  // The minimum of a signed type has the magnitude of its maximum plus one, e.g. INT_MIN = -2^31.
  if (magnitude > (negative ? maximum + 1U : maximum)) {
    throwOutOfRange();
  }
  if constexpr (std::is_signed_v<ValueT_>) {
    if (negative && magnitude != 0U) {
      return static_cast<ValueT_>(-static_cast<ValueT_>(magnitude - 1U) - ValueT_{1});
    }
  }
  return static_cast<ValueT_>(magnitude);
}

//! base^exponent of a non-negative integer and a non-negative exponent, or nothing if it does not fit into std::uintmax_t.
inline std::optional<std::uintmax_t> checkedPower(const std::uintmax_t base, const int exponent) {
  std::uintmax_t result = 1U;
  // The powers of 0 and 1 do not change after the first factor, so the loop stops there rather than run up to the exponent.
  for (int index = 0; index < exponent && (index == 0 || base > 1U); ++index) {
    if (base > 1U && result > std::numeric_limits<std::uintmax_t>::max() / base) {
      return std::nullopt;
    }
    result *= base;
  }
  return result;
}

//! Whether root^degree <= radicand, exactly.
inline bool powerIsAtMost(const std::uintmax_t root, const int degree, const std::uintmax_t radicand) {
  const std::optional<std::uintmax_t> power = checkedPower(root, degree);
  return power.has_value() && *power <= radicand;
}

//! The integer root floor(radicand^(1/degree)), exactly: the floating point estimate of the root is corrected with exact powers,
//! e.g. from 3.9999999999999996 to 4 for 8^(2/3), the cube root of 64, and from 10^9 to 10^9 - 1 for the square root of
//! 10^18 - 1, which double rounds to 10^18. The degree is at least 2, so the root and its estimate are at most 2^32.
inline std::uintmax_t floorRoot(const std::uintmax_t radicand, const int degree, const double estimate) {
  auto root = static_cast<std::uintmax_t>(estimate);
  while (root > 0U && !powerIsAtMost(root, degree, radicand)) {
    --root;
  }
  while (powerIsAtMost(root + 1U, degree, radicand)) {
    ++root;
  }
  return root;
}

//! The power base^(P/Q) of a floating point value for the reduced exponent ExponentT_ = P/Q, with std::pow, or with std::sqrt and
//! std::cbrt for 1/2 and 1/3, which are more accurate: e.g. the exponent 1/3 is not exact in floating point, so
//! std::pow(64.0, 1.0 / 3.0) is 3.9999999999999996, while std::cbrt(64.0) is 4.
template <typename ExponentT_, typename FloatingPointT_>
FloatingPointT_ floatingPointPower(const FloatingPointT_ base) {
  if constexpr (std::is_same_v<ExponentT_, Rational<1, 2>>) {
    return std::sqrt(base);
  } else if constexpr (std::is_same_v<ExponentT_, Rational<1, 3>>) {
    return std::cbrt(base);
  } else {
    return std::pow(base, ExponentT_::template value<FloatingPointT_>());
  }
}

/**
 * The real power base^(P/Q) of an integer for the reduced exponent ExponentT_ = P/Q, truncated toward zero, exactly: the root of
 * base^P is estimated in floating point and corrected with exact powers (see floorRoot). Only if base^P does not fit into 64
 * bits, the floating point power is truncated. A negative base has a real power if Q is odd, (-1)^P |base|^(P/Q).
 * @throws std::domain_error For a negative base and an even Q, which has no real power, and for zero and a negative P, whose power
 * is infinite: converting NaN or infinity into an integer is undefined behavior.
 * @throws std::range_error If the power does not fit into the value type, e.g. pow<2>() of 2^16 with int values.
 */
template <typename ExponentT_, typename ValueT_>
ValueT_ integerPower(const ValueT_ base) {
  constexpr int DividendV = ExponentT_::Dividend;
  constexpr int DivisorV = ExponentT_::Divisor;
  bool negative = false;
  if constexpr (std::is_signed_v<ValueT_>) {
    negative = base < ValueT_{0};
  }
  if constexpr (DivisorV % 2 == 0) {
    if (negative) {
      throw std::domain_error("A negative integer value has no real power with an exponent whose reduced denominator is even.");
    }
  }
  const std::uintmax_t magnitude = magnitudeOf(base);
  // The sign (-1)^P of a negative base. It and the root below are chosen at compile time, like the sign in realPower: branches on
  // P and Q at runtime would leave code that coverage reports as never executed, e.g. the sign of a negative base for an even Q,
  // which has thrown already.
  bool negativeResult = false;
  if constexpr (DividendV % 2 != 0) {
    negativeResult = negative;
  }
  if constexpr (DividendV < 0) {
    if (magnitude == 0U) {
      throw std::domain_error("An integer zero has no power with a negative exponent: it is infinite.");
    }
    // 1 / |base|^(-P/Q) is truncated to 0, unless |base| is 1.
    return fromMagnitude<ValueT_>(negativeResult, magnitude == 1U ? 1U : 0U);
  } else {
    const double estimate = floatingPointPower<ExponentT_>(static_cast<double>(magnitude));
    const std::optional<std::uintmax_t> radicand = checkedPower(magnitude, DividendV);
    if (!radicand.has_value()) {
      return fromMagnitude<ValueT_>(negativeResult, truncateEstimate(estimate));
    }
    if constexpr (DivisorV == 1) {
      return fromMagnitude<ValueT_>(negativeResult, *radicand);
    } else {
      return fromMagnitude<ValueT_>(negativeResult, floorRoot(*radicand, DivisorV, estimate));
    }
  }
}

/**
 * The real power base^(P/Q) of a value for the reduced exponent ExponentT_ = P/Q. A negative base has a real power if Q is odd:
 * (-1)^P |base|^(P/Q), e.g. (-8)^(1/3) = -2 and (-8)^(2/3) = 4, like std::cbrt computes the cube root, whereas std::pow is NaN for
 * every negative base and exponent that is not an integer (https://en.cppreference.com/w/cpp/numeric/math/pow#Error_handling). If
 * Q is even, a negative base has none: floating point values give NaN, like std::pow and std::sqrt. The powers of floating point
 * values are computed in their type, those of integers exactly (see integerPower).
 */
template <typename ExponentT_, typename ValueT_>
ValueT_ realPower(const ValueT_ base) {
  if constexpr (std::is_integral_v<ValueT_>) {
    return integerPower<ExponentT_>(base);
  } else {
    if constexpr (ExponentT_::Divisor % 2 != 0) {
      if (base < ValueT_{0}) {
        // The sign (-1)^P is exactly 1 or -1 for the integer P. Unlike a branch on P, which is known at compile time, it leaves no
        // code that coverage reports as never executed.
        return static_cast<ValueT_>(std::pow(ValueT_{-1}, ExponentT_::Dividend) * floatingPointPower<ExponentT_>(-base));
      }
    }
    return floatingPointPower<ExponentT_>(base);
  }
}

//! The hypotenuse sqrt(x^2 + y^2) of two values, like std::hypot. For integer values, it is truncated exactly (see floorRoot),
//! e.g. that of 2 10^8 and 2 10^4 is 200000000, the floor of sqrt((2 10^8 + 1)^2 - 1), which std::hypot rounds to 200000001.
//! Only if x^2 + y^2 does not fit into 64 bits, the floating point hypotenuse is truncated.
//! @throws std::range_error For integer values whose hypotenuse does not fit into the value type.
template <typename ValueT_>
ValueT_ realHypot(const ValueT_ x, const ValueT_ y) {
  if constexpr (std::is_integral_v<ValueT_>) {
    // Up to sqrt(2) 2^64 for 64-bit unsigned values, which does not fit into 64 bits (see truncateEstimate).
    const double estimate = std::hypot(static_cast<double>(x), static_cast<double>(y));
    const std::optional<std::uintmax_t> xSquared = checkedPower(magnitudeOf(x), 2);
    const std::optional<std::uintmax_t> ySquared = checkedPower(magnitudeOf(y), 2);
    if (xSquared.has_value() && ySquared.has_value() && *xSquared <= std::numeric_limits<std::uintmax_t>::max() - *ySquared) {
      return fromMagnitude<ValueT_>(false, floorRoot(*xSquared + *ySquared, 2, estimate));
    }
    return fromMagnitude<ValueT_>(false, truncateEstimate(estimate));
  } else {
    return std::hypot(x, y);
  }
}

}  // namespace wuzp::internal
