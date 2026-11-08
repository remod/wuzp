#pragma once

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {
namespace internal {

// Translations move points and leave vectors unchanged. The translation is given as quantities: the translations check the
// coordinate systems themselves. The result is a new tensor, so a translation never changes its input.
// These are overloads rather than one function with `if constexpr`, like mapRelative(): MSVC compiles both branches of an
// `if constexpr` in member functions of class templates, and the sum of a point and a translation does not compile for vectors
// or point clouds.

//! A vector is not translated.
template <typename QuantitiesT_, typename CoordinateSystemT_, typename TranslationQuantitiesT_>
auto translate(const Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, Relative>& vector, const TranslationQuantitiesT_& /*translation*/) {
  return vector.evaluate();
}

//! A point is translated by adding the translation: p' = p + t. The sum checks the units of every component, which may differ.
template <typename QuantitiesT_, typename CoordinateSystemT_, typename TranslationQuantitiesT_>
  requires(Cols<QuantitiesT_>::value == 1)
auto translate(const Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, Absolute>& point, const TranslationQuantitiesT_& translation) {
  return typename Evaluate<Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, Absolute>>::type{point.getQuantities() + translation};
}

//! A point cloud, whose columns are points, is translated by adding the translation to every column. The quantities must be
//! homogeneous.
template <typename QuantitiesT_, typename CoordinateSystemT_, typename TranslationQuantitiesT_>
  requires(Cols<QuantitiesT_>::value > 1)
auto translate(const Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, Absolute>& points, const TranslationQuantitiesT_& translation) {
  auto result = points.evaluate();
  // TODO(remod): Improve efficiency by performing this addition at compile time.
  for (int row = 0; row < Rows<QuantitiesT_>::value; ++row) {
    for (int col = 0; col < Cols<QuantitiesT_>::value; ++col) {
      result.setQuantity(row, col, result.getQuantity(row, col) + translation.get(row, 0));
    }
  }
  return result;
}

}  // namespace internal
}  // namespace wuzp
