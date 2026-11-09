#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/trafo/ActiveTransformation.hpp"
#include "wuzp/trafo/internal/map_relative.hpp"
#include "wuzp/trafo/internal/with_quantities.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, AnyCoordinateSystem CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_>
  requires AnyValueMatrix<ValuesT_>
class ActiveLinearTransformation
    : public ActiveTransformation<
          ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>> {
 public:
  using ValuesT = ValuesT_<ValueT_, DimensionToV_, DimensionFromV_>;
  using UnitsT = TypeMatrix<DimensionToV_, DimensionFromV_, UnitsT_...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<Indices<Up, Down>, QuantitiesT, CoordinateSystemT, Relative>;

  // Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  template <typename... Args>
    requires(!CopiesOrMoves<ActiveLinearTransformation, Args...>)
  explicit ActiveLinearTransformation(Args&&... args) : quantities_{std::forward<Args>(args)...} {}

  //! The matrix A, which transforms a vector v into A v.
  const QuantitiesT& getQuantities() const { return quantities_; }
  //! The matrix as a linear map, a tensor with an upper and a lower index; only for a square matrix.
  TensorT getTensor() const
    requires(DimensionFromV_ == DimensionToV_)
  {
    return TensorT{quantities_};
  }

 private:
  //! The matrix A, which transforms a vector v into A v. It is multiplied with the quantities of the tensors it transforms (see
  //! internal::withQuantities()).
  QuantitiesT quantities_;
};

}  // namespace wuzp

#include "wuzp/trafo/ActiveTranslation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_>
struct GetCoordinateSystem<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>> {
  using type = CoordinateSystemT_;
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename NewCoordinateSystemT_>
struct CastCoordinateSystem<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            NewCoordinateSystemT_> {
  using type = ActiveLinearTransformation<ValuesT_, ValueT_, NewCoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>;
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& transformation,
      const Tensor<Indices<Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    // A covector transforms with the inverse: w' = w A^-1.
    return internal::mapRelative(object, [&transformation](const auto& relative) {
      return internal::withQuantities<CoordinateSystemT_>(
          relative, innerProduct(relative.getQuantities(), transformation.getQuantities().tensorInverse()));
    });
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Down, Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& transformation,
      const Tensor<Indices<Down, Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    // A tensor with two lower indices, e.g. a mass matrix, transforms like M' = A^-T M A^-1.
    const auto inverse = transformation.getQuantities().tensorInverse();
    return internal::withQuantities<CoordinateSystemT_>(object,
                                                        innerProduct(inverse.transpose(), innerProduct(object.getQuantities(), inverse)));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& transformation,
      const Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    // A vector transforms like v' = A v.
    return internal::mapRelative(object, [&transformation](const auto& relative) {
      return internal::withQuantities<CoordinateSystemT_>(relative, innerProduct(transformation.getQuantities(), relative.getQuantities()));
    });
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Up, Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& transformation,
      const Tensor<Indices<Up, Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    // A linear map transforms like T' = A T A^-1.
    return internal::withQuantities<CoordinateSystemT_>(
        object,
        innerProduct(transformation.getQuantities(), innerProduct(object.getQuantities(), transformation.getQuantities().tensorInverse())));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Up, Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& transformation,
      const Tensor<Indices<Up, Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    // A tensor with two upper indices, e.g. a covariance matrix, transforms like T' = A T A^T.
    return internal::withQuantities<CoordinateSystemT_>(
        object,
        innerProduct(innerProduct(transformation.getQuantities(), object.getQuantities()), transformation.getQuantities().transpose()));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, template <typename, int, int> typename ValuesOtherT_, int DimensionOtherV_,
          typename ValueOtherT_, typename CoordinateSystemOtherT_, typename... UnitsOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& transformation,
      const ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>& object) {
    return ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_,
          typename CoordinateSystemOtherT_, int DimensionFromOtherV_, int DimensionToOtherV_, typename... UnitsOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            ActiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionFromOtherV_,
                                                       DimensionToOtherV_, UnitsOtherT_...>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& transformation,
      const ActiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionFromOtherV_, DimensionToOtherV_,
                                       UnitsOtherT_...>& object) {
    return ActiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionFromOtherV_, DimensionToOtherV_,
                                      UnitsOtherT_...>(transformation.transform(object.getTensor()).getQuantities());
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionFromLhsV_,
          int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
bool operator==(const ActiveLinearTransformation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionFromLhsV_, DimensionToLhsV_,
                                                 UnitsLhsT_...>& lhs,
                const ActiveLinearTransformation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionFromRhsV_, DimensionToRhsV_,
                                                 UnitsRhsT_...>& rhs) {
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  return lhs.getQuantities() == rhs.getQuantities();
}

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionFromLhsV_,
          int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
bool operator!=(const ActiveLinearTransformation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionFromLhsV_, DimensionToLhsV_,
                                                 UnitsLhsT_...>& lhs,
                const ActiveLinearTransformation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionFromRhsV_, DimensionToRhsV_,
                                                 UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_>
std::ostream& operator<<(
    std::ostream& lhs,
    const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& rhs) {
  lhs << rhs.getQuantities();
  return lhs;
}

}  // namespace wuzp
