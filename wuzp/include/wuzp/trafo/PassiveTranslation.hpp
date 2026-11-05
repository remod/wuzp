#pragma once

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/trafo/PassiveTransformation.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystemT_, typename From,
          typename To, typename... Units>
class PassiveTranslation
    : public PassiveTransformation<PassiveTranslation<ValuesT_, Dimension, Value, CoordinateSystemT_, From, To, Units...>, From, To> {
 public:
  using ValuesT = ValuesT_<Value, Dimension, 1>;
  using UnitsT = TypeMatrix<Dimension, 1, Units...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<1, 0, QuantitiesT, CoordinateSystemT, Relative>;

  template <typename... Args>
  explicit PassiveTranslation(Args&&... args) : tensor_{std::forward<Args>(args)...} {}

  const TensorT& getTensor() const { return tensor_; }

 private:
  //! Vector from the origin of coordinate system To to the origin of coordinate system From, expressed in coordinate system
  //! CoordinateSystemT_. Adding it to a vector in the coordinate system From transforms it into the coordinate system To.
  TensorT tensor_;
};

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename From,
          typename To, typename... Units>
struct GetCoordinateSystem<PassiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, From, To, Units...>> {
  using type = CoordinateSystem;
};

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename From,
          typename To, typename... Units, typename NewCoordinateSystem>
struct CastCoordinateSystem<PassiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, From, To, Units...>, NewCoordinateSystem> {
  using type = PassiveTranslation<ValuesT_, Dimension, Value, NewCoordinateSystem, From, To, Units...>;
};

}  // namespace wuzp

#include "wuzp/trafo/PassiveLinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename From,
          typename To, typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<PassiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, From, To, Units...>,
                            Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(const PassiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, From, To, Units...>& transformation,
                        const Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    if constexpr (IsAbsolute<Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>>::value) {
      auto fromResult = object;
      // TODO(remod): Improve efficiency by performing this addition at compile time.
      for (int row = 0; row < Rows<Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>>::value; ++row) {
        for (int col = 0; col < Cols<Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>>::value; ++col) {
          fromResult.setQuantity(row, col, fromResult.getQuantity(row, col) + transformation.getTensor().getQuantity(row, 0));
        }
      }
      const auto toResult = typename CastCoordinateSystem<std::decay_t<decltype(fromResult)>, To>::type{fromResult.getQuantities()};
      return toResult.evaluate();
    } else {
      const auto fromResult = object;
      const auto toResult = typename CastCoordinateSystem<std::decay_t<decltype(fromResult)>, To>::type{fromResult.getQuantities()};
      return toResult.evaluate();
    }
  }
};

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename From,
          typename To, typename... Units, template <typename, int, int> typename ValuesOtherT_, int DimensionOther, typename ValueOther,
          typename CoordinateSystemOther, typename FromOther, typename ToOther, typename... UnitsOther>
struct TransformationTraits<
    PassiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, From, To, Units...>,
    PassiveTranslation<ValuesOtherT_, DimensionOther, ValueOther, CoordinateSystemOther, FromOther, ToOther, UnitsOther...>> {
  static auto transform(const PassiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, From, To, Units...>& transformation,
                        const PassiveTranslation<ValuesOtherT_, DimensionOther, ValueOther, CoordinateSystemOther, FromOther, ToOther,
                                                 UnitsOther...>& object) {
    return PassiveTranslation<ValuesOtherT_, DimensionOther, ValueOther, To, FromOther, ToOther, UnitsOther...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename From,
          typename To, typename... Units, template <typename, int, int> typename ValuesOtherT_, typename ValueOther,
          typename CoordinateSystemOther, typename FromOther, typename ToOther, int DimensionFromOther, int DimensionToOther,
          typename... UnitsOther>
struct TransformationTraits<PassiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, From, To, Units...>,
                            PassiveLinearTransformation<ValuesOtherT_, ValueOther, CoordinateSystemOther, FromOther, ToOther,
                                                        DimensionFromOther, DimensionToOther, UnitsOther...>> {
  static auto transform(const PassiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, From, To, Units...>& /*transformation*/,
                        const PassiveLinearTransformation<ValuesOtherT_, ValueOther, CoordinateSystemOther, FromOther, ToOther,
                                                          DimensionFromOther, DimensionToOther, UnitsOther...>& object) {
    // TODO(remod): We could use castCoordinateSystem instead, but we need to specialize it.
    return PassiveLinearTransformation<ValuesOtherT_, ValueOther, To, FromOther, ToOther, DimensionFromOther, DimensionToOther,
                                       UnitsOther...>(object.getTensor().getValues());
  }
};

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhs, typename ValueLhs, typename CoordinateSystemLhs,
          typename FromLhs, typename ToLhs, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, int DimensionRhs,
          typename ValueRhs, typename CoordinateSystemRhs, typename FromRhs, typename ToRhs, typename... UnitsRhsT_>
bool operator==(const PassiveTranslation<ValuesLhsT_, DimensionLhs, ValueLhs, CoordinateSystemLhs, FromLhs, ToLhs, UnitsLhsT_...>& lhs,
                const PassiveTranslation<ValuesRhsT_, DimensionRhs, ValueRhs, CoordinateSystemRhs, FromRhs, ToRhs, UnitsRhsT_...>& rhs) {
  static_assert(std::is_same<FromLhs, FromRhs>::value, "From coordinates frames do not match.");
  static_assert(std::is_same<ToLhs, ToRhs>::value, "To coordinates frames do not match.");
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhs, typename ValueLhs, typename CoordinateSystemLhs,
          typename FromLhs, typename ToLhs, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, int DimensionRhs,
          typename ValueRhs, typename CoordinateSystemRhs, typename FromRhs, typename ToRhs, typename... UnitsRhsT_>
bool operator!=(const PassiveTranslation<ValuesLhsT_, DimensionLhs, ValueLhs, CoordinateSystemLhs, FromLhs, ToLhs, UnitsLhsT_...>& lhs,
                const PassiveTranslation<ValuesRhsT_, DimensionRhs, ValueRhs, CoordinateSystemRhs, FromRhs, ToRhs, UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename From,
          typename To, typename... Units>
std::ostream& operator<<(std::ostream& lhs,
                         const PassiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, From, To, Units...>& rhs) {
  lhs << rhs.getTensor();
  return lhs;
}

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Value, typename CoordinateSystem, typename From,
          typename To>
using PassiveSpatialTranslation =
    PassiveTranslation<ValuesT_, Dimension, Value, CoordinateSystem, From, To, typename AccessUnits<AccessImplementation<Value>>::Length>;
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To>
using PassiveSpatialTranslation2 = PassiveSpatialTranslation<ValuesT_, 2, Value, CoordinateSystem, From, To>;
template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To>
using PassiveSpatialTranslation3 = PassiveSpatialTranslation<ValuesT_, 3, Value, CoordinateSystem, From, To>;

// v == 0 -> Translation
// a == 0 -> Uniform motion
// da/dt == 0 -> Motion with constant acceleration.

// omega == 0 -> Rotation
// phi == 0 -> Uniform circular motion
// phi/dt == 0 -> Circular motion with constant acceleration.

}  // namespace wuzp
