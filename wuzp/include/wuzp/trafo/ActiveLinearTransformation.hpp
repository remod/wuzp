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

template <template <typename, int, int> typename ValuesT_, typename Value, AnyCoordinateSystem CoordinateSystemT_, int DimensionFrom,
          int DimensionTo, typename... Units>
class ActiveLinearTransformation
    : public ActiveTransformation<ActiveLinearTransformation<ValuesT_, Value, CoordinateSystemT_, DimensionFrom, DimensionTo, Units...>> {
 public:
  using ValuesT = ValuesT_<Value, DimensionTo, DimensionFrom>;
  using UnitsT = TypeMatrix<DimensionTo, DimensionFrom, Units...>;
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

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units>
struct GetCoordinateSystem<ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>> {
  using type = CoordinateSystem;
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, typename NewCoordinateSystem>
struct CastCoordinateSystem<ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            NewCoordinateSystem> {
  using type = ActiveLinearTransformation<ValuesT_, Value, NewCoordinateSystem, DimensionFrom, DimensionTo, Units...>;
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<0, 1, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<0, 1, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    // A covector transforms with the inverse: w' = w A^-1.
    return internal::mapRelative(object, [&transformation](const auto& relative) {
      return innerProduct(relative, transformation.getTensor().tensorInverse()).evaluate();
    });
  }
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<0, 2, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<0, 2, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    return innerProduct(transformation.getTensor().tensorInverse().transpose(),
                        innerProduct(object, transformation.getTensor().tensorInverse()))
        .evaluate();
  }
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    return internal::mapRelative(
        object, [&transformation](const auto& relative) { return innerProduct(transformation.getTensor(), relative).evaluate(); });
  }
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<1, 1, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<1, 1, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    return innerProduct(transformation.getTensor(), innerProduct(object, transformation.getTensor().tensorInverse())).evaluate();
  }
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<2, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<2, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    // A tensor with two contravariant indices, e.g. a covariance matrix, transforms like T' = A T A^T.
    // The first product is evaluated, since Blitz++ cannot nest products.
    return innerProduct(innerProduct(transformation.getTensor(), object).evaluate(), transformation.getTensor().transpose()).evaluate();
  }
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, template <typename, int, int> typename ValuesOtherT_, int DimensionOther, typename ValueOther,
          typename CoordinateSystemOther, typename... UnitsOther>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            ActiveTranslation<ValuesOtherT_, DimensionOther, ValueOther, CoordinateSystemOther, UnitsOther...>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const ActiveTranslation<ValuesOtherT_, DimensionOther, ValueOther, CoordinateSystemOther, UnitsOther...>& object) {
    return ActiveTranslation<ValuesOtherT_, DimensionOther, ValueOther, CoordinateSystemOther, UnitsOther...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, template <typename, int, int> typename ValuesOtherT_, typename ValueOther, typename CoordinateSystemOther,
          int DimensionFromOther, int DimensionToOther, typename... UnitsOther>
struct TransformationTraits<
    ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
    ActiveLinearTransformation<ValuesOtherT_, ValueOther, CoordinateSystemOther, DimensionFromOther, DimensionToOther, UnitsOther...>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const ActiveLinearTransformation<ValuesOtherT_, ValueOther, CoordinateSystemOther, DimensionFromOther, DimensionToOther,
                                       UnitsOther...>& object) {
    return ActiveLinearTransformation<ValuesOtherT_, ValueOther, CoordinateSystemOther, DimensionFromOther, DimensionToOther,
                                      UnitsOther...>(transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhs, typename CoordinateSystemLhs, int DimensionFromLhs,
          int DimensionToLhs, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhs,
          typename CoordinateSystemRhs, int DimensionFromRhs, int DimensionToRhs, typename... UnitsRhsT_>
bool operator==(
    const ActiveLinearTransformation<ValuesLhsT_, ValueLhs, CoordinateSystemLhs, DimensionFromLhs, DimensionToLhs, UnitsLhsT_...>& lhs,
    const ActiveLinearTransformation<ValuesRhsT_, ValueRhs, CoordinateSystemRhs, DimensionFromRhs, DimensionToRhs, UnitsRhsT_...>& rhs) {
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhs, typename CoordinateSystemLhs, int DimensionFromLhs,
          int DimensionToLhs, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhs,
          typename CoordinateSystemRhs, int DimensionFromRhs, int DimensionToRhs, typename... UnitsRhsT_>
bool operator!=(
    const ActiveLinearTransformation<ValuesLhsT_, ValueLhs, CoordinateSystemLhs, DimensionFromLhs, DimensionToLhs, UnitsLhsT_...>& lhs,
    const ActiveLinearTransformation<ValuesRhsT_, ValueRhs, CoordinateSystemRhs, DimensionFromRhs, DimensionToRhs, UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units>
std::ostream& operator<<(std::ostream& lhs,
                         const ActiveLinearTransformation<ValuesT_, Value, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& rhs) {
  lhs << rhs.getTensor();
  return lhs;
}

}  // namespace wuzp
