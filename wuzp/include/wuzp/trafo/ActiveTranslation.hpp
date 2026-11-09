#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/trafo/ActiveTransformation.hpp"
#include "wuzp/trafo/internal/translate.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, AnyCoordinateSystem CoordinateSystemT_,
          typename... UnitsT_>
  requires AnyValueMatrix<ValuesT_>
class ActiveTranslation : public ActiveTransformation<ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>> {
 public:
  using ValuesT = ValuesT_<ValueT_, DimensionV_, 1>;
  using UnitsT = TypeMatrix<DimensionV_, 1, UnitsT_...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<1, 0, QuantitiesT, CoordinateSystemT, Relative>;

  // Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  template <typename... Args>
    requires(!CopiesOrMoves<ActiveTranslation, Args...>)
  explicit ActiveTranslation(Args&&... args) : tensor_{std::forward<Args>(args)...} {}

  const TensorT& getTensor() const { return tensor_; }

 private:
  //! Vector which transforms another vector by being added to it.
  TensorT tensor_;
};

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_,
          typename... UnitsT_>
struct GetCoordinateSystem<ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>> {
  using type = CoordinateSystemT_;
};

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_,
          typename... UnitsT_, typename NewCoordinateSystemT_>
struct CastCoordinateSystem<ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>, NewCoordinateSystemT_> {
  using type = ActiveTranslation<ValuesT_, DimensionV_, ValueT_, NewCoordinateSystemT_, UnitsT_...>;
};

}  // namespace wuzp

#include "wuzp/trafo/ActiveLinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_,
          typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
struct TransformationTraits<ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>,
                            Tensor<1, 0, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static auto transform(const ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>& transformation,
                        const Tensor<1, 0, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    static_assert(std::is_same_v<CoordinateSystemT_, CoordinateSystemOtherT_>, "Coordinate systems do not match.");
    return internal::translate(object, transformation.getTensor().getQuantities());
  }
};

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_,
          typename... UnitsT_, template <typename, int, int> typename ValuesOtherT_, int DimensionOtherV_, typename ValueOtherT_,
          typename CoordinateSystemOtherT_, typename... UnitsOtherT_>
struct TransformationTraits<ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>,
                            ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>> {
  static auto transform(
      const ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>& transformation,
      const ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>& object) {
    return ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_,
          typename... UnitsT_, template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_,
          typename CoordinateSystemOtherT_, int DimensionFromOtherV_, int DimensionToOtherV_, typename... UnitsOtherT_>
struct TransformationTraits<ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>,
                            ActiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionFromOtherV_,
                                                       DimensionToOtherV_, UnitsOtherT_...>> {
  static auto transform(const ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>& /*transformation*/,
                        const ActiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionFromOtherV_,
                                                         DimensionToOtherV_, UnitsOtherT_...>& object) {
    return object;
  }
};

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename CoordinateSystemLhsT_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, int DimensionRhsV_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, typename... UnitsRhsT_>
bool operator==(const ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>& lhs,
                const ActiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, UnitsRhsT_...>& rhs) {
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename CoordinateSystemLhsT_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, int DimensionRhsV_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, typename... UnitsRhsT_>
bool operator!=(const ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>& lhs,
                const ActiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_,
          typename... UnitsT_>
std::ostream& operator<<(std::ostream& lhs, const ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, UnitsT_...>& rhs) {
  lhs << rhs.getTensor();
  return lhs;
}

template <template <typename, int, int> typename ValuesT_, int DimensionV_, typename ValueT_, typename CoordinateSystemT_>
using ActiveSpatialTranslation =
    ActiveTranslation<ValuesT_, DimensionV_, ValueT_, CoordinateSystemT_, typename AccessUnits<AccessImplementation<ValueT_>>::Length>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ActiveSpatialTranslation2 = ActiveSpatialTranslation<ValuesT_, 2, ValueT_, CoordinateSystemT_>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ActiveSpatialTranslation3 = ActiveSpatialTranslation<ValuesT_, 3, ValueT_, CoordinateSystemT_>;

}  // namespace wuzp
