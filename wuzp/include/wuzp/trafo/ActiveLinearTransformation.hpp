#pragma once

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/trafo/ActiveTransformation.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystemT_, int DimensionFrom, int DimensionTo,
          typename... Units>
class ActiveLinearTransformation
    : public ActiveTransformation<ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystemT_, DimensionFrom, DimensionTo, Units...>> {
 public:
  using ValuesT = ValuesT_<Scalar, DimensionTo, DimensionFrom>;
  using UnitsT = TypeMatrix<DimensionTo, DimensionFrom, Units...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<1, 1, QuantitiesT, CoordinateSystemT, Relative>;

  template <typename... Args>
  explicit ActiveLinearTransformation(Args... args) : tensor_(args...) {}

  const TensorT& getTensor() const { return tensor_; }

 private:
  //! Tensor holding the orientation (= active rotation) of coordinate system To expressed in coordinate system From.
  TensorT tensor_;
};

}  // namespace wuzp

#include "wuzp/trafo/ActiveTranslation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units>
struct GetCoordinateSystem<ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>> {
  using type = CoordinateSystem;
};

template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, typename NewCoordinateSystem>
struct CastCoordinateSystem<ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            NewCoordinateSystem> {
  using type = ActiveLinearTransformation<ValuesT_, Scalar, NewCoordinateSystem, DimensionFrom, DimensionTo, Units...>;
};

template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<0, 1, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& /*transformation*/,
      const Tensor<0, 1, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& /*object*/) {
    //    return transformation.getTensor() * object;
  }
};

template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<0, 2, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<0, 2, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    const auto result = transformation.getTensor().inverse().transpose() * object * transformation.getTensor().inverse();
    return evaluate(result);
  }
};

template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    // If the object is absolute (point), make it relative (vector), transform it, and make it absolute again.
    if constexpr (IsAbsolute<Tensor<1, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>>::value) {
      return evaluate(makeAbsolute(transformation.getTensor() * makeRelative(object)));
    } else {
      return evaluate(transformation.getTensor() * object);
    }
  }
};

template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<1, 1, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<1, 1, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& object) {
    return transformation.getTensor() * (object * transformation.getTensor().inverse());
  }
};

template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, typename QuantitiesT_, typename CoordinateSystemT_, typename AbsoluteT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<2, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& /*transformation*/,
      const Tensor<2, 0, QuantitiesT_, CoordinateSystemT_, AbsoluteT_>& /*object*/) {
    //    return transformation.getTensor() * object;
  }
};

template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, template <typename, int, int> typename ValuesOtherT_, int DimensionOther, typename ScalarOther,
          typename CoordinateSystemOther, typename... UnitsOther>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            ActiveTranslation<ValuesOtherT_, DimensionOther, ScalarOther, CoordinateSystemOther, UnitsOther...>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const ActiveTranslation<ValuesOtherT_, DimensionOther, ScalarOther, CoordinateSystemOther, UnitsOther...>& object) {
    return ActiveTranslation<ValuesOtherT_, DimensionOther, ScalarOther, CoordinateSystemOther, UnitsOther...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units, template <typename, int, int> typename ValuesOtherT_, typename ScalarOther, typename CoordinateSystemOther,
          int DimensionFromOther, int DimensionToOther, typename... UnitsOther>
struct TransformationTraits<
    ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
    ActiveLinearTransformation<ValuesOtherT_, ScalarOther, CoordinateSystemOther, DimensionFromOther, DimensionToOther, UnitsOther...>> {
  static auto transform(
      const ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const ActiveLinearTransformation<ValuesOtherT_, ScalarOther, CoordinateSystemOther, DimensionFromOther, DimensionToOther,
                                       UnitsOther...>& object) {
    return ActiveLinearTransformation<ValuesOtherT_, ScalarOther, CoordinateSystemOther, DimensionFromOther, DimensionToOther,
                                      UnitsOther...>(transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ScalarLhs, typename CoordinateSystemLhs, int DimensionFromLhs,
          int DimensionToLhs, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ScalarRhs,
          typename CoordinateSystemRhs, int DimensionFromRhs, int DimensionToRhs, typename... UnitsRhsT_>
bool operator==(
    const ActiveLinearTransformation<ValuesLhsT_, ScalarLhs, CoordinateSystemLhs, DimensionFromLhs, DimensionToLhs, UnitsLhsT_...>& lhs,
    const ActiveLinearTransformation<ValuesRhsT_, ScalarRhs, CoordinateSystemRhs, DimensionFromRhs, DimensionToRhs, UnitsRhsT_...>& rhs) {
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValuesLhsT_, typename ScalarLhs, typename CoordinateSystemLhs, int DimensionFromLhs,
          int DimensionToLhs, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ScalarRhs,
          typename CoordinateSystemRhs, int DimensionFromRhs, int DimensionToRhs, typename... UnitsRhsT_>
bool operator!=(
    const ActiveLinearTransformation<ValuesLhsT_, ScalarLhs, CoordinateSystemLhs, DimensionFromLhs, DimensionToLhs, UnitsLhsT_...>& lhs,
    const ActiveLinearTransformation<ValuesRhsT_, ScalarRhs, CoordinateSystemRhs, DimensionFromRhs, DimensionToRhs, UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, typename Scalar, typename CoordinateSystem, int DimensionFrom, int DimensionTo,
          typename... Units>
std::ostream& operator<<(std::ostream& lhs,
                         const ActiveLinearTransformation<ValuesT_, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& rhs) {
  lhs << rhs.getTensor();
  return lhs;
}

}  // namespace wuzp
