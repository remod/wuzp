#pragma once

#include <ostream>
#include <utility>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/trafo/ActiveTransformation.hpp"
#include "wuzp/trafo/internal/map_relative.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, AnyCoordinateSystem CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_>
class ActiveLinearTransformation
    : public ActiveTransformation<
          ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>> {
 public:
  using ValuesT = ValuesT_<ValueT_, DimensionToV_, DimensionFromV_>;
  using UnitsT = TypeMatrix<DimensionToV_, DimensionFromV_, UnitsT_...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<1, 1, QuantitiesT, CoordinateSystemT, Relative>;

  // Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  template <typename... Args>
    requires(!CopiesOrMoves<ActiveLinearTransformation, Args...>)
  explicit ActiveLinearTransformation(Args&&... args) : tensor_{std::forward<Args>(args)...} {}

  const TensorT& getTensor() const { return tensor_; }

 private:
  //! Tensor which transforms a vector by being contracted with it.
  TensorT tensor_;
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
                            Tensor<0, 1, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& transformation,
      const Tensor<0, 1, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    // A covector transforms with the inverse: w' = w A^-1.
    return internal::mapRelative(
        object, [&transformation](const auto& relative) { return innerProduct(relative, transformation.getTensor().tensorInverse()); });
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<0, 2, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& transformation,
      const Tensor<0, 2, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    return innerProduct(transformation.getTensor().tensorInverse().transpose(),
                        innerProduct(object, transformation.getTensor().tensorInverse()));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<1, 0, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& transformation,
      const Tensor<1, 0, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    return internal::mapRelative(object,
                                 [&transformation](const auto& relative) { return innerProduct(transformation.getTensor(), relative); });
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<1, 1, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& transformation,
      const Tensor<1, 1, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    return innerProduct(transformation.getTensor(), innerProduct(object, transformation.getTensor().tensorInverse()));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<2, 0, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& transformation,
      const Tensor<2, 0, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    // A tensor with two contravariant indices, e.g. a covariance matrix, transforms like T' = A T A^T.
    return innerProduct(innerProduct(transformation.getTensor(), object), transformation.getTensor().transpose());
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
                                      UnitsOtherT_...>(transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionFromLhsV_,
          int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
bool operator==(const ActiveLinearTransformation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionFromLhsV_, DimensionToLhsV_,
                                                 UnitsLhsT_...>& lhs,
                const ActiveLinearTransformation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionFromRhsV_, DimensionToRhsV_,
                                                 UnitsRhsT_...>& rhs) {
  return lhs.getTensor() == rhs.getTensor();
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
  lhs << rhs.getTensor();
  return lhs;
}

}  // namespace wuzp
