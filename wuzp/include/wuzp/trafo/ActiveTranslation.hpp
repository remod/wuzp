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

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, AnyCoordinateSystem CoordinateSystemT_,
          typename... Units>
class ActiveTranslation : public ActiveTransformation<ActiveTranslation<ValuesT_, Dimension, Value, CoordinateSystemT_, Units...>> {
 public:
  using ValuesT = ValuesT_<Value, Dimension, 1>;
  using UnitsT = TypeMatrix<Dimension, 1, Units...>;
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

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename... Units>
struct GetCoordinateSystem<ActiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, Units...>> {
  using type = CoordinateSystem;
};

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename... Units,
          typename NewCoordinateSystem>
struct CastCoordinateSystem<ActiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, Units...>, NewCoordinateSystem> {
  using type = ActiveTranslation<ValuesT_, Dimension, Value, NewCoordinateSystem, Units...>;
};

}  // namespace wuzp

#include "wuzp/trafo/ActiveLinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename... Units,
          typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<ActiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, Units...>,
                            Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(const ActiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, Units...>& transformation,
                        const Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    static_assert(std::is_same_v<CoordinateSystem, CoordinateSystemT_>, "Coordinate systems do not match.");
    return internal::translate(object, transformation.getTensor().getQuantities());
  }
};

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename... Units,
          template <typename, int, int> typename ValuesOtherT_, int DimensionOther, typename ValueOther, typename CoordinateSystemOther,
          typename... UnitsOther>
struct TransformationTraits<ActiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, Units...>,
                            ActiveTranslation<ValuesOtherT_, DimensionOther, ValueOther, CoordinateSystemOther, UnitsOther...>> {
  static auto transform(const ActiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, Units...>& transformation,
                        const ActiveTranslation<ValuesOtherT_, DimensionOther, ValueOther, CoordinateSystemOther, UnitsOther...>& object) {
    return ActiveTranslation<ValuesOtherT_, DimensionOther, ValueOther, CoordinateSystemOther, UnitsOther...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename... Units,
          template <typename, int, int> typename ValuesOtherT_, typename ValueOther, typename CoordinateSystemOther, int DimensionFromOther,
          int DimensionToOther, typename... UnitsOther>
struct TransformationTraits<
    ActiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, Units...>,
    ActiveLinearTransformation<ValuesOtherT_, ValueOther, CoordinateSystemOther, DimensionFromOther, DimensionToOther, UnitsOther...>> {
  static auto transform(const ActiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, Units...>& /*transformation*/,
                        const ActiveLinearTransformation<ValuesOtherT_, ValueOther, CoordinateSystemOther, DimensionFromOther,
                                                         DimensionToOther, UnitsOther...>& object) {
    return object;
  }
};

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhs, typename ValueLhs, typename CoordinateSystemLhs,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, int DimensionRhs, typename ValueRhs,
          typename CoordinateSystemRhs, typename... UnitsRhsT_>
bool operator==(const ActiveTranslation<ValuesLhsT_, DimensionLhs, ValueLhs, CoordinateSystemLhs, UnitsLhsT_...>& lhs,
                const ActiveTranslation<ValuesRhsT_, DimensionRhs, ValueRhs, CoordinateSystemRhs, UnitsRhsT_...>& rhs) {
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhs, typename ValueLhs, typename CoordinateSystemLhs,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, int DimensionRhs, typename ValueRhs,
          typename CoordinateSystemRhs, typename... UnitsRhsT_>
bool operator!=(const ActiveTranslation<ValuesLhsT_, DimensionLhs, ValueLhs, CoordinateSystemLhs, UnitsLhsT_...>& lhs,
                const ActiveTranslation<ValuesRhsT_, DimensionRhs, ValueRhs, CoordinateSystemRhs, UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename... Units>
std::ostream& operator<<(std::ostream& lhs, const ActiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, Units...>& rhs) {
  lhs << rhs.getTensor();
  return lhs;
}

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem>
using ActiveSpatialTranslation =
    ActiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, typename AccessUnits<AccessImplementation<Value>>::Length>;
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem>
using ActiveSpatialTranslation2 = ActiveSpatialTranslation<ValuesT_, 2, Value, CoordinateSystem>;
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem>
using ActiveSpatialTranslation3 = ActiveSpatialTranslation<ValuesT_, 3, Value, CoordinateSystem>;

}  // namespace wuzp
