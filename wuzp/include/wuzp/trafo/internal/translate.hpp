#pragma once

#include <type_traits>
#include <utility>

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {
namespace internal {

// Translations move points and leave vectors, covectors and tensors with two upper indices unchanged (see "Translations of
// tensors" in DESIGN.md). The translation is given as quantities: the translations check the coordinate systems themselves. The
// result is a new tensor, so a translation never changes its input. It is expressed in ResultCoordinateSystemT_: the coordinate
// system of the object for an active translation, To for a passive one, whose axes are those of From (see PassiveTranslation).
// These are overloads rather than one function with `if constexpr`: MSVC compiles both branches of an `if constexpr` in member
// functions of class templates, and the sum of a point and a translation does not compile for vectors or point clouds.

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

//! The dimension of a tensor that a translation leaves unchanged, which must be that of the translation, like that of a point,
//! whose sum with the translation checks it: the number of rows of a vector or a tensor with two upper indices, and the number of
//! columns of a covector, which is stored as a row (see Tensor).
template <typename IndicesT_, typename QuantitiesT_>
inline constexpr int translationInvariantDimension =
    std::is_same_v<IndicesT_, Indices<Down>> ? Cols<QuantitiesT_>::value : Rows<QuantitiesT_>::value;

//! The result of the translation of a tensor of the type TensorT_: the evaluated tensor in ResultCoordinateSystemT_. The
//! translations construct it in this coordinate system, rather than casting it into it, which would move the values again.
template <typename ResultCoordinateSystemT_, typename TensorT_>
using Translated = typename Evaluate<typename CastCoordinateSystem<TensorT_, ResultCoordinateSystemT_>::type>::type;

//! A vector, a covector or a tensor with two upper indices is not translated: the result has its components, which are copied, or,
//! if it is a temporary, e.g. the vector that the rotation of a proper rigid transformation returns, moved. Other relative tensors
//! are rejected (see isTranslationInvariant), and so are tensors of another dimension than the translation.
template <typename ResultCoordinateSystemT_, typename TensorT_, typename TranslationQuantitiesT_>
  requires(AnyTensor<std::remove_cvref_t<TensorT_>> && std::is_same_v<typename std::remove_cvref_t<TensorT_>::AbsoluteT, Relative>)
auto translate(TensorT_&& tensor, const TranslationQuantitiesT_& /*translation*/) {
  using TensorT = std::remove_cvref_t<TensorT_>;
  static_assert(isTranslationInvariant<typename TensorT::IndicesT>,
                "A translation only transforms points, vectors, covectors and tensors with two upper indices: other tensors may depend "
                "on the origin, e.g. an inertia about it.");
  static_assert(
      translationInvariantDimension<typename TensorT::IndicesT, typename TensorT::QuantitiesT> == Rows<TranslationQuantitiesT_>::value,
      "Matrix sizes do not match.");
  return Translated<ResultCoordinateSystemT_, TensorT>{std::forward<TensorT_>(tensor).getQuantities()};
}

//! A point is translated by adding the translation: p' = p + t. The sum checks the units of every component, which may differ.
template <typename ResultCoordinateSystemT_, typename QuantitiesT_, typename CoordinateSystemT_, typename TranslationQuantitiesT_>
  requires(Cols<QuantitiesT_>::value == 1)
auto translate(const Tensor<Indices<Up>, QuantitiesT_, CoordinateSystemT_, Absolute>& point, const TranslationQuantitiesT_& translation) {
  return Translated<ResultCoordinateSystemT_, Tensor<Indices<Up>, QuantitiesT_, CoordinateSystemT_, Absolute>>{point.getQuantities() +
                                                                                                               translation};
}

/**
 * A point cloud, whose columns are points, is translated by adding the translation to every column: P' = P + t 1^T, where 1^T is
 * a row of ones, so that every column of t 1^T is t. The units of this sum are checked at compile time, like those of the sum of a
 * single point and the translation, so the components of a point may have different units, e.g. a length and an angle. The values
 * are then added element by element: unlike computing t 1^T and the sum with the backend, this needs no values besides those of
 * the result, e.g. no other arrays with Blitz++, and unlike adding quantities element by element, no quantity for every element.
 */
template <typename ResultCoordinateSystemT_, typename QuantitiesT_, typename CoordinateSystemT_, typename TranslationQuantitiesT_>
  requires(Cols<QuantitiesT_>::value > 1)
auto translate(const Tensor<Indices<Up>, QuantitiesT_, CoordinateSystemT_, Absolute>& points, const TranslationQuantitiesT_& translation) {
  using OnesUnitsT =
      TypeMatrix<1, Cols<QuantitiesT_>::value, typename AccessUnits<AccessImplementation<typename QuantitiesT_::ValueT>>::Dimensionless>;
  using UnitsT =
      typename Sum<typename QuantitiesT_::UnitsT, typename InnerProduct<typename TranslationQuantitiesT_::UnitsT, OnesUnitsT>::type>::type;
  auto values = points.getValues().evaluate();
  const auto& translationValues = translation.getValues();
  for (int row = 0; row < Rows<QuantitiesT_>::value; ++row) {
    for (int col = 0; col < Cols<QuantitiesT_>::value; ++col) {
      values.set(row, col, values.get(row, col) + translationValues.get(row, 0));
    }
  }
  return Translated<ResultCoordinateSystemT_, Tensor<Indices<Up>, Quantities<decltype(values), UnitsT>, CoordinateSystemT_, Absolute>>{
      std::move(values)};
}

}  // namespace internal
}  // namespace wuzp
