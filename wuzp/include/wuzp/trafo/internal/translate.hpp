#pragma once

#include <type_traits>

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {
namespace internal {

// Translations move points and leave vectors, covectors and tensors with two upper indices unchanged (see "Translations of
// tensors" in DESIGN.md). The translation is given as quantities: the translations check the coordinate systems themselves. The
// result is a new tensor, so a translation never changes its input.
// These are overloads rather than one function with `if constexpr`, like mapRelative(): MSVC compiles both branches of an
// `if constexpr` in member functions of class templates, and the sum of a point and a translation does not compile for vectors
// or point clouds.

/**
 * Whether a translation leaves the relative tensors with these indices unchanged: vectors, e.g. velocities, covectors, e.g.
 * gradients, and tensors with two upper indices, e.g. covariances. The differential of a translation is the identity, so it does
 * not change a tensor at a point, whose components do not depend on the origin. The type of a tensor cannot tell a tensor at a
 * point from a quantity about a point, e.g. an inertia about the origin, which changes with the origin by the parallel axis
 * theorem, so the other tensors, e.g. with two lower indices like an inertia, are rejected.
 */
template <typename IndicesT_>
inline constexpr bool isTranslationInvariant =
    std::is_same_v<IndicesT_, Indices<Up>> || std::is_same_v<IndicesT_, Indices<Down>> || std::is_same_v<IndicesT_, Indices<Up, Up>>;

//! A vector, a covector or a tensor with two upper indices is not translated. Other relative tensors are rejected (see
//! isTranslationInvariant).
template <typename IndicesT_, typename QuantitiesT_, typename CoordinateSystemT_, typename TranslationQuantitiesT_>
auto translate(const Tensor<IndicesT_, QuantitiesT_, CoordinateSystemT_, Relative>& tensor,
               const TranslationQuantitiesT_& /*translation*/) {
  static_assert(isTranslationInvariant<IndicesT_>,
                "A translation only transforms points, vectors, covectors and tensors with two upper indices: other tensors may depend "
                "on the origin, e.g. an inertia about it.");
  return tensor.evaluate();
}

//! A point is translated by adding the translation: p' = p + t. The sum checks the units of every component, which may differ.
template <typename QuantitiesT_, typename CoordinateSystemT_, typename TranslationQuantitiesT_>
  requires(Cols<QuantitiesT_>::value == 1)
auto translate(const Tensor<Indices<Up>, QuantitiesT_, CoordinateSystemT_, Absolute>& point, const TranslationQuantitiesT_& translation) {
  return typename Evaluate<Tensor<Indices<Up>, QuantitiesT_, CoordinateSystemT_, Absolute>>::type{point.getQuantities() + translation};
}

//! A point cloud, whose columns are points, is translated by adding the translation to every column. The quantities must be
//! homogeneous.
template <typename QuantitiesT_, typename CoordinateSystemT_, typename TranslationQuantitiesT_>
  requires(Cols<QuantitiesT_>::value > 1)
auto translate(const Tensor<Indices<Up>, QuantitiesT_, CoordinateSystemT_, Absolute>& points, const TranslationQuantitiesT_& translation) {
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
