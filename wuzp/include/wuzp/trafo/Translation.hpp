#pragma once

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystemT_, typename From,
          typename To, typename... Units>
class Translation : public PassiveTransformation<Translation<ValueMatrix, Dimension, Scalar, CoordinateSystemT_, From, To, Units...>,
                                                 CoordinateSystemT_, From, To> {
 public:
  using ValueMatrixT = ValueMatrix<Scalar, Dimension, 1>;
  using UnitMatrixT = TypeMatrix<Dimension, 1, Units...>;
  using QuantityMatrixT = QuantityMatrix<ValueMatrixT, UnitMatrixT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<1, 0, QuantityMatrixT, CoordinateSystemT>;
  static_assert(!IsAbsolute<UnitMatrixT>::value, "Units are not relative.");

  template <typename... Args>
  explicit Translation(Args... args) : tensor_(args...) {}
  //  template <typename... Args>
  //  explicit Translation(Args&&... args) : tensor_(std::forward<Args>(args)...) {}

  const TensorT& getTensor() const { return tensor_; }

 private:
  TensorT tensor_;
};

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To, typename... Units>
struct GetCoordinateSystem<Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>> {
  using type = CoordinateSystem;
};

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To, typename... Units, typename NewCoordinateSystem>
struct CastCoordinateSystem<Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>, NewCoordinateSystem> {
  using type = Translation<ValueMatrix, Dimension, Scalar, NewCoordinateSystem, From, To, Units...>;
};

}  // namespace wuzp

#include "wuzp/trafo/LinearTransformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To, typename... Units, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>,
                            Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(const Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>& transformation,
                        const Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>& object) {
    if constexpr (IsAbsolute<Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>>::value) {
      const auto fromResult = transformation.getTensor() + object;
      const auto toResult = typename CastCoordinateSystem<std::decay_t<decltype(fromResult)>, To>::type(fromResult.getQuantityMatrix());
      return evaluate(toResult);
    } else {
      const auto fromResult = object;
      const auto toResult = typename CastCoordinateSystem<std::decay_t<decltype(fromResult)>, To>::type(fromResult.getQuantityMatrix());
      return evaluate(toResult);
    }
  }
};

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To, typename... Units, template <typename, int, int> typename ValueMatrixOther, int DimensionOther, typename ScalarOther,
          typename CoordinateSystemOther, typename FromOther, typename ToOther, typename... UnitsOther>
struct TransformationTraits<
    Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>,
    Translation<ValueMatrixOther, DimensionOther, ScalarOther, CoordinateSystemOther, FromOther, ToOther, UnitsOther...>> {
  static auto transform(
      const Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>& transformation,
      const Translation<ValueMatrixOther, DimensionOther, ScalarOther, CoordinateSystemOther, FromOther, ToOther, UnitsOther...>& object) {
    return Translation<ValueMatrixOther, DimensionOther, ScalarOther, To, FromOther, ToOther, UnitsOther...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To, typename... Units, template <typename, int, int> typename ValueMatrixOther, typename ScalarOther,
          typename CoordinateSystemOther, typename FromOther, typename ToOther, int DimensionFromOther, int DimensionToOther,
          typename... UnitsOther>
struct TransformationTraits<Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>,
                            LinearTransformation<ValueMatrixOther, ScalarOther, CoordinateSystemOther, FromOther, ToOther,
                                                 DimensionFromOther, DimensionToOther, UnitsOther...>> {
  static auto transform(const Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>& /*transformation*/,
                        const LinearTransformation<ValueMatrixOther, ScalarOther, CoordinateSystemOther, FromOther, ToOther,
                                                   DimensionFromOther, DimensionToOther, UnitsOther...>& object) {
    // TODO(remod): We could use castCoordinateSystem instead, but we need to specialize it.
    return LinearTransformation<ValueMatrixOther, ScalarOther, To, FromOther, ToOther, DimensionFromOther, DimensionToOther, UnitsOther...>(
        object.getTensor().getValueMatrix());
  }
};

template <template <typename, int, int> typename ValueMatrixLhs, int DimensionLhs, typename ScalarLhs, typename CoordinateSystemLhs,
          typename FromLhs, typename ToLhs, typename... UnitsLhs, template <typename, int, int> typename ValueMatrixRhs, int DimensionRhs,
          typename ScalarRhs, typename CoordinateSystemRhs, typename FromRhs, typename ToRhs, typename... UnitsRhs>
bool operator==(const Translation<ValueMatrixLhs, DimensionLhs, ScalarLhs, CoordinateSystemLhs, FromLhs, ToLhs, UnitsLhs...>& lhs,
                const Translation<ValueMatrixRhs, DimensionRhs, ScalarRhs, CoordinateSystemRhs, FromRhs, ToRhs, UnitsRhs...>& rhs) {
  static_assert(std::is_same<FromLhs, FromRhs>::value, "From coordinates frames do not match.");
  static_assert(std::is_same<ToLhs, ToRhs>::value, "To coordinates frames do not match.");
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValueMatrixLhs, int DimensionLhs, typename ScalarLhs, typename CoordinateSystemLhs,
          typename FromLhs, typename ToLhs, typename... UnitsLhs, template <typename, int, int> typename ValueMatrixRhs, int DimensionRhs,
          typename ScalarRhs, typename CoordinateSystemRhs, typename FromRhs, typename ToRhs, typename... UnitsRhs>
bool operator!=(const Translation<ValueMatrixLhs, DimensionLhs, ScalarLhs, CoordinateSystemLhs, FromLhs, ToLhs, UnitsLhs...>& lhs,
                const Translation<ValueMatrixRhs, DimensionRhs, ScalarRhs, CoordinateSystemRhs, FromRhs, ToRhs, UnitsRhs...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To, typename... Units>
std::ostream& operator<<(std::ostream& stream,
                         const Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>& translation) {
  stream << translation.getTensor();
  return stream;
}

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename From,
          typename To>
using SpatialTranslation =
    Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, typename AccessUnits<SpecializationDummy<Scalar>>::Length>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To>
using SpatialTranslation2 = SpatialTranslation<ValueMatrix, 2, Scalar, CoordinateSystem, From, To>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To>
using SpatialTranslation3 = SpatialTranslation<ValueMatrix, 3, Scalar, CoordinateSystem, From, To>;

// v == 0 -> Translation
// a == 0 -> Uniform motion
// da/dt == 0 -> Motion with constant acceleration.

}  // namespace wuzp
