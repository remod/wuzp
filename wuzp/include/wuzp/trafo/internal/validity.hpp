#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "wuzp/core/traits.hpp"

namespace wuzp {
namespace internal {

/**
 * Whether the constructors of rotations and Lorentz transformations check that they are valid: in debug builds, i.e. if NDEBUG
 * is not defined, like assert() (see "Validity of rotations and Lorentz transformations" in DESIGN.md). Release builds skip the
 * check, which costs a matrix product, like Drake's RotationMatrix
 * (https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rotation_matrix.html). Like with assert(), all translation units of a
 * program must agree on NDEBUG.
 */
#ifdef NDEBUG
inline constexpr bool checksValidity = false;
#else
inline constexpr bool checksValidity = true;
#endif

//! Throws std::invalid_argument with the message if the transformation is not valid (see its isValid()), in debug builds.
template <typename TransformationT_>
void checkValidity(const TransformationT_& transformation, const char* message) {
  if constexpr (checksValidity) {
    if (!transformation.isValid()) {
      throw std::invalid_argument(message);
    }
  }
}

//! The identity matrix, as values of the type ValuesT_.
template <typename ValuesT_>
ValuesT_ identity() {
  ValuesT_ result = ValuesT_::zero();
  for (int index = 0; index < Rows<ValuesT_>::value; ++index) {
    result.set(index, index, typename ValuesT_::ValueT{1});
  }
  return result;
}

//! The largest absolute difference of the elements of two matrices of the same size, or NaN if a difference is NaN, so that a
//! comparison with a tolerance fails.
template <typename ValuesT_>
typename ValuesT_::ValueT maxAbsDifference(const ValuesT_& lhs, const ValuesT_& rhs) {
  using ValueT = typename ValuesT_::ValueT;
  ValueT result{0};
  for (int row = 0; row < Rows<ValuesT_>::value; ++row) {
    for (int col = 0; col < Cols<ValuesT_>::value; ++col) {
      const ValueT difference = std::abs(lhs.get(row, col) - rhs.get(row, col));
      if (std::isnan(difference)) {
        return difference;
      }
      result = std::max(result, difference);
    }
  }
  return result;
}

}  // namespace internal
}  // namespace wuzp
