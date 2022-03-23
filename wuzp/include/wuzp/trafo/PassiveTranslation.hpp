#pragma once

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Scalar, typename CoordinateSystemT_, typename From,
          typename To, typename... Units>
class PassiveTranslation
    : public PassiveTransformation<PassiveTranslation<ValuesT_, Dimension, Scalar, CoordinateSystemT_, From, To, Units...>, From, To> {
 public:
  using ValuesT = ValuesT_<Scalar, Dimension, 1>;
  using UnitMatrixT = TypeMatrix<Dimension, 1, Units...>;
  using QuantityMatrixT = QuantityMatrix<ValuesT, UnitMatrixT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<1, 0, QuantityMatrixT, CoordinateSystemT>;
  static_assert(!IsAbsolute<UnitMatrixT>::value, "Units are not relative.");

  template <typename... Args>
  explicit PassiveTranslation(Args... args) : tensor_(args...) {}
  //  template <typename... Args>
  //  explicit PassiveTranslation(Args&&... args) : tensor_(std::forward<Args>(args)...) {}

  const TensorT& getTensor() const { return tensor_; }

 private:
  //! Tensor holding the position (= active translation) of coordinate system To expressed in coordinate system From.
  TensorT tensor_;
};

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To, typename... Units>
struct GetCoordinateSystem<PassiveTranslation<ValuesT_, Dimension, Scalar, CoordinateSystem, From, To, Units...>> {
  using type = CoordinateSystem;
};

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To, typename... Units, typename NewCoordinateSystem>
struct CastCoordinateSystem<PassiveTranslation<ValuesT_, Dimension, Scalar, CoordinateSystem, From, To, Units...>, NewCoordinateSystem> {
  using type = PassiveTranslation<ValuesT_, Dimension, Scalar, NewCoordinateSystem, From, To, Units...>;
};

}  // namespace wuzp

#include "wuzp/trafo/PassiveLinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To, typename... Units, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<PassiveTranslation<ValuesT_, Dimension, Scalar, CoordinateSystem, From, To, Units...>,
                            Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(const PassiveTranslation<ValuesT_, Dimension, Scalar, CoordinateSystem, From, To, Units...>& transformation,
                        const Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>& object) {
    if constexpr (IsAbsolute<Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>>::value) {
      auto fromResult = object;
      // TODO(remod): Improve efficiency by performing this addition at compile time.
      for (int row = 0; row < Rows<Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>>::value; ++row) {
        for (int col = 0; col < Cols<Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>>::value; ++col) {
          fromResult.template setQuantity(row, col, fromResult.getQuantity(row, col) + transformation.getTensor().getQuantity(row, 0));
        }
      }
      const auto toResult = typename CastCoordinateSystem<std::decay_t<decltype(fromResult)>, To>::type(fromResult.getQuantityMatrix());
      return evaluate(toResult);
    } else {
      const auto fromResult = object;
      const auto toResult = typename CastCoordinateSystem<std::decay_t<decltype(fromResult)>, To>::type(fromResult.getQuantityMatrix());
      return evaluate(toResult);
    }
  }
};

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To, typename... Units, template <typename, int, int> typename ValuesOtherT_, int DimensionOther, typename ScalarOther,
          typename CoordinateSystemOther, typename FromOther, typename ToOther, typename... UnitsOther>
struct TransformationTraits<
    PassiveTranslation<ValuesT_, Dimension, Scalar, CoordinateSystem, From, To, Units...>,
    PassiveTranslation<ValuesOtherT_, DimensionOther, ScalarOther, CoordinateSystemOther, FromOther, ToOther, UnitsOther...>> {
  static auto transform(const PassiveTranslation<ValuesT_, Dimension, Scalar, CoordinateSystem, From, To, Units...>& transformation,
                        const PassiveTranslation<ValuesOtherT_, DimensionOther, ScalarOther, CoordinateSystemOther, FromOther, ToOther,
                                                 UnitsOther...>& object) {
    return PassiveTranslation<ValuesOtherT_, DimensionOther, ScalarOther, To, FromOther, ToOther, UnitsOther...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To, typename... Units, template <typename, int, int> typename ValuesOtherT_, typename ScalarOther,
          typename CoordinateSystemOther, typename FromOther, typename ToOther, int DimensionFromOther, int DimensionToOther,
          typename... UnitsOther>
struct TransformationTraits<PassiveTranslation<ValuesT_, Dimension, Scalar, CoordinateSystem, From, To, Units...>,
                            PassiveLinearTransformation<ValuesOtherT_, ScalarOther, CoordinateSystemOther, FromOther, ToOther,
                                                        DimensionFromOther, DimensionToOther, UnitsOther...>> {
  static auto transform(const PassiveTranslation<ValuesT_, Dimension, Scalar, CoordinateSystem, From, To, Units...>& /*transformation*/,
                        const PassiveLinearTransformation<ValuesOtherT_, ScalarOther, CoordinateSystemOther, FromOther, ToOther,
                                                          DimensionFromOther, DimensionToOther, UnitsOther...>& object) {
    // TODO(remod): We could use castCoordinateSystem instead, but we need to specialize it.
    return PassiveLinearTransformation<ValuesOtherT_, ScalarOther, To, FromOther, ToOther, DimensionFromOther, DimensionToOther,
                                       UnitsOther...>(object.getTensor().getValues());
  }
};

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhs, typename ScalarLhs, typename CoordinateSystemLhs,
          typename FromLhs, typename ToLhs, typename... UnitsLhs, template <typename, int, int> typename ValuesRhsT_, int DimensionRhs,
          typename ScalarRhs, typename CoordinateSystemRhs, typename FromRhs, typename ToRhs, typename... UnitsRhs>
bool operator==(const PassiveTranslation<ValuesLhsT_, DimensionLhs, ScalarLhs, CoordinateSystemLhs, FromLhs, ToLhs, UnitsLhs...>& lhs,
                const PassiveTranslation<ValuesRhsT_, DimensionRhs, ScalarRhs, CoordinateSystemRhs, FromRhs, ToRhs, UnitsRhs...>& rhs) {
  static_assert(std::is_same<FromLhs, FromRhs>::value, "From coordinates frames do not match.");
  static_assert(std::is_same<ToLhs, ToRhs>::value, "To coordinates frames do not match.");
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhs, typename ScalarLhs, typename CoordinateSystemLhs,
          typename FromLhs, typename ToLhs, typename... UnitsLhs, template <typename, int, int> typename ValuesRhsT_, int DimensionRhs,
          typename ScalarRhs, typename CoordinateSystemRhs, typename FromRhs, typename ToRhs, typename... UnitsRhs>
bool operator!=(const PassiveTranslation<ValuesLhsT_, DimensionLhs, ScalarLhs, CoordinateSystemLhs, FromLhs, ToLhs, UnitsLhs...>& lhs,
                const PassiveTranslation<ValuesRhsT_, DimensionRhs, ScalarRhs, CoordinateSystemRhs, FromRhs, ToRhs, UnitsRhs...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To, typename... Units>
std::ostream& operator<<(std::ostream& lhs,
                         const PassiveTranslation<ValuesT_, Dimension, Scalar, CoordinateSystem, From, To, Units...>& rhs) {
  lhs << rhs.getTensor();
  return lhs;
}

template <template <typename, int, int> typename ValuesT_, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To>
using PassiveSpatialTranslation =
    PassiveTranslation<ValuesT_, Dimension, Scalar, CoordinateSystem, From, To, typename AccessUnits<SpecializationDummy<Scalar>>::Length>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, typename From, typename To>
using PassiveSpatialTranslation2 = PassiveSpatialTranslation<ValuesT_, 2, Scalar, CoordinateSystem, From, To>;
template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, typename From, typename To>
using PassiveSpatialTranslation3 = PassiveSpatialTranslation<ValuesT_, 3, Scalar, CoordinateSystem, From, To>;

// v == 0 -> Translation
// a == 0 -> Uniform motion
// da/dt == 0 -> Motion with constant acceleration.

// omega == 0 -> Rotation
// phi == 0 -> Uniform circular motion
// phi/dt == 0 -> Circular motion with constant acceleration.

}  // namespace wuzp
