#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/trafo/PassiveTransformation.hpp"
#include "wuzp/trafo/internal/map_relative.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, AnyCoordinateSystem CoordinateSystemT_,
          AnyCoordinateSystem FromT_, AnyCoordinateSystem ToT_, int DimensionFromV_, int DimensionToV_, typename... UnitsT_>
class PassiveLinearTransformation
    : public PassiveTransformation<
          PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
          FromT_, ToT_> {
 public:
  using ValuesT = ValuesT_<ValueT_, DimensionToV_, DimensionFromV_>;
  using UnitsT = TypeMatrix<DimensionToV_, DimensionFromV_, UnitsT_...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<1, 1, QuantitiesT, CoordinateSystemT, Relative>;

  // Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  template <typename... Args>
    requires(!CopiesOrMoves<PassiveLinearTransformation, Args...>)
  explicit PassiveLinearTransformation(Args&&... args) : tensor_{std::forward<Args>(args)...} {}

  const TensorT& getTensor() const { return tensor_; }

 private:
  //! Tensor composed of the stacked transposed basis vectors of coordinate system To expressed in coordinate system From.
  //! Contracting it with a vector in the coordinate system From transforms it into the coordinate system To.
  TensorT tensor_;
};

}  // namespace wuzp

#include "wuzp/trafo/PassiveTranslation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_>
struct GetCoordinateSystem<
    PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>> {
  using type = CoordinateSystemT_;
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_, typename NewCoordinateSystemT_>
struct CastCoordinateSystem<
    PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
    NewCoordinateSystemT_> {
  using type =
      PassiveLinearTransformation<ValuesT_, ValueT_, NewCoordinateSystemT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>;
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
          typename AbsoluteOtherT_>
struct TransformationTraits<
    PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
    Tensor<0, 1, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(const PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_,
                                                          DimensionToV_, UnitsT_...>& transformation,
                        const Tensor<0, 1, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    // A covector transforms with the inverse: w' = w A^-1.
    return castCoordinateSystem<ToT_>(internal::mapRelative(
        object, [&transformation](const auto& relative) { return innerProduct(relative, transformation.getTensor().tensorInverse()); }));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
          typename AbsoluteOtherT_>
struct TransformationTraits<
    PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
    Tensor<0, 2, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(const PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_,
                                                          DimensionToV_, UnitsT_...>& transformation,
                        const Tensor<0, 2, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    return castCoordinateSystem<ToT_>(innerProduct(transformation.getTensor().tensorInverse().transpose(),
                                                   innerProduct(object, transformation.getTensor().tensorInverse())));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
          typename AbsoluteOtherT_>
struct TransformationTraits<
    PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
    Tensor<1, 0, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(const PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_,
                                                          DimensionToV_, UnitsT_...>& transformation,
                        const Tensor<1, 0, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    return castCoordinateSystem<ToT_>(internal::mapRelative(
        object, [&transformation](const auto& relative) { return innerProduct(transformation.getTensor(), relative); }));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
          typename AbsoluteOtherT_>
struct TransformationTraits<
    PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
    Tensor<1, 1, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(const PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_,
                                                          DimensionToV_, UnitsT_...>& transformation,
                        const Tensor<1, 1, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    return castCoordinateSystem<ToT_>(
        innerProduct(transformation.getTensor(), innerProduct(object, transformation.getTensor().tensorInverse())));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
          typename AbsoluteOtherT_>
struct TransformationTraits<
    PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
    Tensor<2, 0, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(const PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_,
                                                          DimensionToV_, UnitsT_...>& transformation,
                        const Tensor<2, 0, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    // A tensor with two contravariant indices, e.g. a covariance matrix, transforms like T' = A T A^T.
    return castCoordinateSystem<ToT_>(
        innerProduct(innerProduct(transformation.getTensor(), object), transformation.getTensor().transpose()));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_, template <typename, int, int> typename ValuesOtherT_,
          int DimensionOtherV_, typename ValueOtherT_, typename CoordinateSystemOtherT_, typename FromOtherT_, typename ToOtherT_,
          typename... UnitsOtherT_>
struct TransformationTraits<
    PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
    PassiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, FromOtherT_, ToOtherT_, UnitsOtherT_...>> {
  static auto transform(const PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_,
                                                          DimensionToV_, UnitsT_...>& transformation,
                        const PassiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, FromOtherT_,
                                                 ToOtherT_, UnitsOtherT_...>& object) {
    return PassiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, ToT_, FromOtherT_, ToOtherT_, UnitsOtherT_...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_, template <typename, int, int> typename ValuesOtherT_,
          typename ValueOtherT_, typename CoordinateSystemOtherT_, typename FromOtherT_, typename ToOtherT_, int DimensionFromOtherV_,
          int DimensionToOtherV_, typename... UnitsOtherT_>
struct TransformationTraits<
    PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
    PassiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, FromOtherT_, ToOtherT_, DimensionFromOtherV_,
                                DimensionToOtherV_, UnitsOtherT_...>> {
  static auto transform(const PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_, DimensionFromV_,
                                                          DimensionToV_, UnitsT_...>& transformation,
                        const PassiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, FromOtherT_, ToOtherT_,
                                                          DimensionFromOtherV_, DimensionToOtherV_, UnitsOtherT_...>& object) {
    return PassiveLinearTransformation<ValuesOtherT_, ValueOtherT_, ToT_, FromOtherT_, ToOtherT_, DimensionFromOtherV_, DimensionToOtherV_,
                                       UnitsOtherT_...>(transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, typename FromLhsT_,
          typename ToLhsT_, int DimensionFromLhsV_, int DimensionToLhsV_, typename... UnitsLhsT_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename CoordinateSystemRhsT_, typename FromRhsT_,
          typename ToRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
bool operator==(const PassiveLinearTransformation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, FromLhsT_, ToLhsT_, DimensionFromLhsV_,
                                                  DimensionToLhsV_, UnitsLhsT_...>& lhs,
                const PassiveLinearTransformation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, FromRhsT_, ToRhsT_, DimensionFromRhsV_,
                                                  DimensionToRhsV_, UnitsRhsT_...>& rhs) {
  static_assert(std::is_same<FromLhsT_, FromRhsT_>::value, "'From' coordinate systems do not match.");
  static_assert(std::is_same<ToLhsT_, ToRhsT_>::value, "'To' coordinate systems do not match.");
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, typename FromLhsT_,
          typename ToLhsT_, int DimensionFromLhsV_, int DimensionToLhsV_, typename... UnitsLhsT_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename CoordinateSystemRhsT_, typename FromRhsT_,
          typename ToRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
bool operator!=(const PassiveLinearTransformation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, FromLhsT_, ToLhsT_, DimensionFromLhsV_,
                                                  DimensionToLhsV_, UnitsLhsT_...>& lhs,
                const PassiveLinearTransformation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, FromRhsT_, ToRhsT_, DimensionFromRhsV_,
                                                  DimensionToRhsV_, UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, typename FromT_, typename ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_>
std::ostream& operator<<(std::ostream& lhs, const PassiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, FromT_, ToT_,
                                                                              DimensionFromV_, DimensionToV_, UnitsT_...>& rhs) {
  lhs << rhs.getTensor();
  return lhs;
}

}  // namespace wuzp
