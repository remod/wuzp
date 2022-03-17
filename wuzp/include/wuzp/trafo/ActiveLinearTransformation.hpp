#pragma once

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystemT_, int DimensionFrom,
          int DimensionTo, typename... Units>
class ActiveLinearTransformation
    : public ActiveTransformation<
          ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystemT_, DimensionFrom, DimensionTo, Units...>> {
 public:
  using ValueMatrixT = ValueMatrix<Scalar, DimensionTo, DimensionFrom>;
  using UnitMatrixT = TypeMatrix<DimensionTo, DimensionFrom, Units...>;
  using QuantityMatrixT = QuantityMatrix<ValueMatrixT, UnitMatrixT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<1, 1, QuantityMatrixT, CoordinateSystemT>;

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

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, int DimensionFrom,
          int DimensionTo, typename... Units>
struct GetCoordinateSystem<ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>> {
  using type = CoordinateSystem;
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, int DimensionFrom,
          int DimensionTo, typename... Units, typename NewCoordinateSystem>
struct CastCoordinateSystem<ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            NewCoordinateSystem> {
  using type = ActiveLinearTransformation<ValueMatrix, Scalar, NewCoordinateSystem, DimensionFrom, DimensionTo, Units...>;
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, int DimensionFrom,
          int DimensionTo, typename... Units, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<0, 1, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<0, 1, QuantityMatrixT_, CoordinateSystemT_>& object) {
    //    return transformation.getTensor() * object;
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, int DimensionFrom,
          int DimensionTo, typename... Units, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<0, 2, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<0, 2, QuantityMatrixT_, CoordinateSystemT_>& object) {
    const auto result = transformation.getTensor().inverse().transpose() * object * transformation.getTensor().inverse();
    return evaluate(result);
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, int DimensionFrom,
          int DimensionTo, typename... Units, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>& object) {
    // If the object is absolute (point), make it relative (vector), transform it, and make it absolute again.
    if constexpr (IsAbsolute<Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>>::value) {
      return evaluate(makeAbsolute(transformation.getTensor() * makeRelative(object)));
    } else {
      return evaluate(transformation.getTensor() * object);
    }
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, int DimensionFrom,
          int DimensionTo, typename... Units, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<1, 1, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<1, 1, QuantityMatrixT_, CoordinateSystemT_>& object) {
    return transformation.getTensor() * (object * transformation.getTensor().inverse());
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, int DimensionFrom,
          int DimensionTo, typename... Units, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            Tensor<2, 0, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(
      const ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<2, 0, QuantityMatrixT_, CoordinateSystemT_>& object) {
    //    return transformation.getTensor() * object;
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, int DimensionFrom,
          int DimensionTo, typename... Units, template <typename, int, int> typename ValueMatrixOther, int DimensionOther,
          typename ScalarOther, typename CoordinateSystemOther, typename... UnitsOther>
struct TransformationTraits<ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
                            ActiveTranslation<ValueMatrixOther, DimensionOther, ScalarOther, CoordinateSystemOther, UnitsOther...>> {
  static auto transform(
      const ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const ActiveTranslation<ValueMatrixOther, DimensionOther, ScalarOther, CoordinateSystemOther, UnitsOther...>& object) {
    return ActiveTranslation<ValueMatrixOther, DimensionOther, ScalarOther, CoordinateSystemOther, UnitsOther...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, int DimensionFrom,
          int DimensionTo, typename... Units, template <typename, int, int> typename ValueMatrixOther, typename ScalarOther,
          typename CoordinateSystemOther, int DimensionFromOther, int DimensionToOther, typename... UnitsOther>
struct TransformationTraits<
    ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>,
    ActiveLinearTransformation<ValueMatrixOther, ScalarOther, CoordinateSystemOther, DimensionFromOther, DimensionToOther, UnitsOther...>> {
  static auto transform(
      const ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& transformation,
      const ActiveLinearTransformation<ValueMatrixOther, ScalarOther, CoordinateSystemOther, DimensionFromOther, DimensionToOther,
                                       UnitsOther...>& object) {
    return ActiveLinearTransformation<ValueMatrixOther, ScalarOther, CoordinateSystemOther, DimensionFromOther, DimensionToOther,
                                      UnitsOther...>(transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValueMatrixLhs, typename ScalarLhs, typename CoordinateSystemLhs, int DimensionFromLhs,
          int DimensionToLhs, typename... UnitsLhs, template <typename, int, int> typename ValueMatrixRhs, typename ScalarRhs,
          typename CoordinateSystemRhs, int DimensionFromRhs, int DimensionToRhs, typename... UnitsRhs>
bool operator==(
    const ActiveLinearTransformation<ValueMatrixLhs, ScalarLhs, CoordinateSystemLhs, DimensionFromLhs, DimensionToLhs, UnitsLhs...>& lhs,
    const ActiveLinearTransformation<ValueMatrixRhs, ScalarRhs, CoordinateSystemRhs, DimensionFromRhs, DimensionToRhs, UnitsRhs...>& rhs) {
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValueMatrixLhs, typename ScalarLhs, typename CoordinateSystemLhs, int DimensionFromLhs,
          int DimensionToLhs, typename... UnitsLhs, template <typename, int, int> typename ValueMatrixRhs, typename ScalarRhs,
          typename CoordinateSystemRhs, int DimensionFromRhs, int DimensionToRhs, typename... UnitsRhs>
bool operator!=(
    const ActiveLinearTransformation<ValueMatrixLhs, ScalarLhs, CoordinateSystemLhs, DimensionFromLhs, DimensionToLhs, UnitsLhs...>& lhs,
    const ActiveLinearTransformation<ValueMatrixRhs, ScalarRhs, CoordinateSystemRhs, DimensionFromRhs, DimensionToRhs, UnitsRhs...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, int DimensionFrom,
          int DimensionTo, typename... Units>
std::ostream& operator<<(
    std::ostream& stream,
    const ActiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, DimensionFrom, DimensionTo, Units...>& translation) {
  stream << translation.getTensor();
  return stream;
}

}  // namespace wuzp
