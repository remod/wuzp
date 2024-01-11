#pragma once

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/trafo/PassiveTransformation.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystemT_, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units>
class PassiveLinearTransformation
    : public PassiveTransformation<
          PassiveLinearTransformation<ValuesT_, Value, CoordinateSystemT_, From, To, DimensionFrom, DimensionTo, Units...>, From, To> {
 public:
  using ValuesT = ValuesT_<Value, DimensionTo, DimensionFrom>;
  using UnitsT = TypeMatrix<DimensionTo, DimensionFrom, Units...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<1, 1, QuantitiesT, CoordinateSystemT, Relative>;

  template <typename... Args>
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

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units>
struct GetCoordinateSystem<PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>> {
  using type = CoordinateSystem;
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename NewCoordinateSystem>
struct CastCoordinateSystem<PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            NewCoordinateSystem> {
  using type = PassiveLinearTransformation<ValuesT_, Value, NewCoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>;
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<0, 1, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(const PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo,
                                                          Units...>& /*transformation*/,
                        const Tensor<0, 1, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& /*object*/) {
    // TODO(remod): Implement.
    //    return innerProduct(transformation.getTensor(), object);
  }
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<0, 2, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<0, 2, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    return castCoordinateSystem<To>(innerProduct(transformation.getTensor().tensorInverse().transpose(),
                                                 innerProduct(object, transformation.getTensor().tensorInverse())))
        .evaluate();
  }
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    // If the object is absolute (point), make it relative (vector), transform it, and make it absolute again.
    if constexpr (IsAbsolute<Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>>::value) {
      return (castCoordinateSystem<To>(makeAbsolute(innerProduct(transformation.getTensor(), makeRelative(object))))).evaluate();
    } else {
      return (castCoordinateSystem<To>(innerProduct(transformation.getTensor(), object))).evaluate();
    }
  }
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<1, 1, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<1, 1, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    return castCoordinateSystem<To>(
        innerProduct(transformation.getTensor(), innerProduct(object, transformation.getTensor().tensorInverse())));
  }
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<2, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(const PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo,
                                                          Units...>& /*transformation*/,
                        const Tensor<2, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& /*object*/) {
    // TODO(remod): Implement.
    //    return innerProduct(transformation.getTensor(), object);
  }
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, template <typename, int, int> typename ValuesOtherT_, int DimensionOther,
          typename ValueOther, typename CoordinateSystemOther, typename FromOther, typename ToOther, typename... UnitsOther>
struct TransformationTraits<
    PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
    PassiveTranslation<ValuesOtherT_, DimensionOther, ValueOther, CoordinateSystemOther, FromOther, ToOther, UnitsOther...>> {
  static auto transform(
      const PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const PassiveTranslation<ValuesOtherT_, DimensionOther, ValueOther, CoordinateSystemOther, FromOther, ToOther, UnitsOther...>&
          object) {
    return PassiveTranslation<ValuesOtherT_, DimensionOther, ValueOther, To, FromOther, ToOther, UnitsOther...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, template <typename, int, int> typename ValuesOtherT_, typename ValueOther,
          typename CoordinateSystemOther, typename FromOther, typename ToOther, int DimensionFromOther, int DimensionToOther,
          typename... UnitsOther>
struct TransformationTraits<PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            PassiveLinearTransformation<ValuesOtherT_, ValueOther, CoordinateSystemOther, FromOther, ToOther,
                                                        DimensionFromOther, DimensionToOther, UnitsOther...>> {
  static auto transform(
      const PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const PassiveLinearTransformation<ValuesOtherT_, ValueOther, CoordinateSystemOther, FromOther, ToOther, DimensionFromOther,
                                        DimensionToOther, UnitsOther...>& object) {
    return PassiveLinearTransformation<ValuesOtherT_, ValueOther, To, FromOther, ToOther, DimensionFromOther, DimensionToOther,
                                       UnitsOther...>(transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhs, typename CoordinateSystemLhs, typename FromLhs,
          typename ToLhs, int DimensionFromLhs, int DimensionToLhs, typename... UnitsLhsT_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhs, typename CoordinateSystemRhs, typename FromRhs,
          typename ToRhs, int DimensionFromRhs, int DimensionToRhs, typename... UnitsRhsT_>
bool operator==(const PassiveLinearTransformation<ValuesLhsT_, ValueLhs, CoordinateSystemLhs, FromLhs, ToLhs, DimensionFromLhs,
                                                  DimensionToLhs, UnitsLhsT_...>& lhs,
                const PassiveLinearTransformation<ValuesRhsT_, ValueRhs, CoordinateSystemRhs, FromRhs, ToRhs, DimensionFromRhs,
                                                  DimensionToRhs, UnitsRhsT_...>& rhs) {
  static_assert(std::is_same<FromLhs, FromRhs>::value, "'From' coordinate systems do not match.");
  static_assert(std::is_same<ToLhs, ToRhs>::value, "'To' coordinate systems do not match.");
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhs, typename CoordinateSystemLhs, typename FromLhs,
          typename ToLhs, int DimensionFromLhs, int DimensionToLhs, typename... UnitsLhsT_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhs, typename CoordinateSystemRhs, typename FromRhs,
          typename ToRhs, int DimensionFromRhs, int DimensionToRhs, typename... UnitsRhsT_>
bool operator!=(const PassiveLinearTransformation<ValuesLhsT_, ValueLhs, CoordinateSystemLhs, FromLhs, ToLhs, DimensionFromLhs,
                                                  DimensionToLhs, UnitsLhsT_...>& lhs,
                const PassiveLinearTransformation<ValuesRhsT_, ValueRhs, CoordinateSystemRhs, FromRhs, ToRhs, DimensionFromRhs,
                                                  DimensionToRhs, UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, typename Value, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units>
std::ostream& operator<<(
    std::ostream& lhs,
    const PassiveLinearTransformation<ValuesT_, Value, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& rhs) {
  lhs << rhs.getTensor();
  return lhs;
}

}  // namespace wuzp
