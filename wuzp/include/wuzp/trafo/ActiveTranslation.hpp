#pragma once

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystemT_,
          typename... Units>
class ActiveTranslation : public ActiveTransformation<ActiveTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystemT_, Units...>> {
 public:
  using ValueMatrixT = ValueMatrix<Scalar, Dimension, 1>;
  using UnitMatrixT = TypeMatrix<Dimension, 1, Units...>;
  using QuantityMatrixT = QuantityMatrix<ValueMatrixT, UnitMatrixT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<1, 0, QuantityMatrixT, CoordinateSystemT>;
  static_assert(!IsAbsolute<UnitMatrixT>::value, "Units are not relative.");

  template <typename... Args>
  explicit ActiveTranslation(Args... args) : tensor_(args...) {}
  //  template <typename... Args>
  //  explicit ActiveTranslation(Args&&... args) : tensor_(std::forward<Args>(args)...) {}

  const TensorT& getTensor() const { return tensor_; }

 private:
  //! Tensor holding the position (= active translation) of coordinate system To expressed in coordinate system From.
  TensorT tensor_;
};

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename... Units>
struct GetCoordinateSystem<ActiveTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem, Units...>> {
  using type = CoordinateSystem;
};

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename... Units,
          typename NewCoordinateSystem>
struct CastCoordinateSystem<ActiveTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem, Units...>, NewCoordinateSystem> {
  using type = ActiveTranslation<ValueMatrix, Dimension, Scalar, NewCoordinateSystem, Units...>;
};

}  // namespace wuzp

#include "wuzp/trafo/ActiveLinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename... Units,
          typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<ActiveTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem, Units...>,
                            Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(const ActiveTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem, Units...>& transformation,
                        const Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>& object) {
    if constexpr (IsAbsolute<Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>>::value) {
      auto result = object;
      // TODO(remod): Improve efficiency by performing this addition at compile time.
      for (int row = 0; row < Rows<Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>>::value; ++row) {
        for (int col = 0; col < Cols<Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>>::value; ++col) {
          result.template setQuantity(row, col, result.getQuantity(row, col) + transformation.getTensor().getQuantity(row, 0));
        }
      }
      return evaluate(result);
    } else {
      const auto result = object;
      return evaluate(result);
    }
  }
};

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename... Units,
          template <typename, int, int> typename ValueMatrixOther, int DimensionOther, typename ScalarOther, typename CoordinateSystemOther,
          typename... UnitsOther>
struct TransformationTraits<ActiveTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem, Units...>,
                            ActiveTranslation<ValueMatrixOther, DimensionOther, ScalarOther, CoordinateSystemOther, UnitsOther...>> {
  static auto transform(
      const ActiveTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem, Units...>& transformation,
      const ActiveTranslation<ValueMatrixOther, DimensionOther, ScalarOther, CoordinateSystemOther, UnitsOther...>& object) {
    return ActiveTranslation<ValueMatrixOther, DimensionOther, ScalarOther, CoordinateSystemOther, UnitsOther...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename... Units,
          template <typename, int, int> typename ValueMatrixOther, typename ScalarOther, typename CoordinateSystemOther,
          int DimensionFromOther, int DimensionToOther, typename... UnitsOther>
struct TransformationTraits<
    ActiveTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem, Units...>,
    ActiveLinearTransformation<ValueMatrixOther, ScalarOther, CoordinateSystemOther, DimensionFromOther, DimensionToOther, UnitsOther...>> {
  static auto transform(const ActiveTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem, Units...>& /*transformation*/,
                        const ActiveLinearTransformation<ValueMatrixOther, ScalarOther, CoordinateSystemOther, DimensionFromOther,
                                                         DimensionToOther, UnitsOther...>& object) {
    return object;
  }
};

template <template <typename, int, int> typename ValueMatrixLhs, int DimensionLhs, typename ScalarLhs, typename CoordinateSystemLhs,
          typename... UnitsLhs, template <typename, int, int> typename ValueMatrixRhs, int DimensionRhs, typename ScalarRhs,
          typename CoordinateSystemRhs, typename... UnitsRhs>
bool operator==(const ActiveTranslation<ValueMatrixLhs, DimensionLhs, ScalarLhs, CoordinateSystemLhs, UnitsLhs...>& lhs,
                const ActiveTranslation<ValueMatrixRhs, DimensionRhs, ScalarRhs, CoordinateSystemRhs, UnitsRhs...>& rhs) {
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValueMatrixLhs, int DimensionLhs, typename ScalarLhs, typename CoordinateSystemLhs,
          typename... UnitsLhs, template <typename, int, int> typename ValueMatrixRhs, int DimensionRhs, typename ScalarRhs,
          typename CoordinateSystemRhs, typename... UnitsRhs>
bool operator!=(const ActiveTranslation<ValueMatrixLhs, DimensionLhs, ScalarLhs, CoordinateSystemLhs, UnitsLhs...>& lhs,
                const ActiveTranslation<ValueMatrixRhs, DimensionRhs, ScalarRhs, CoordinateSystemRhs, UnitsRhs...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename... Units>
std::ostream& operator<<(std::ostream& stream,
                         const ActiveTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem, Units...>& translation) {
  stream << translation.getTensor();
  return stream;
}

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem>
using ActiveSpatialTranslation =
    ActiveTranslation<ValueMatrix, Dimension, Scalar, CoordinateSystem, typename AccessUnits<SpecializationDummy<Scalar>>::Length>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using ActiveSpatialTranslation2 = ActiveSpatialTranslation<ValueMatrix, 2, Scalar, CoordinateSystem>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem>
using ActiveSpatialTranslation3 = ActiveSpatialTranslation<ValueMatrix, 3, Scalar, CoordinateSystem>;

}  // namespace wuzp
