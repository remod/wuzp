#pragma once

#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename From, typename To, typename... Units>
class Translation : public PassiveTransformation<Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>, From, To> {
 public:
  using ValueMatrixT = ValueMatrix<Scalar, Dimension, 1>;
  using UnitMatrixT = UnitMatrix<Dimension, 1, Units...>;
  using QuantityMatrixT = QuantityMatrix<ValueMatrixT, UnitMatrixT>;
  using CoordinateSystemMatrixT = CoordinateSystemMatrix<Dimension, 1, CoordinateSystem>;
  using CoordinateMatrixT = CoordinateMatrix<QuantityMatrixT, CoordinateSystemMatrixT>;
  using TensorT = Tensor<1, 0, CoordinateMatrixT>;
  static_assert(!IsAbsolute<UnitMatrixT>::value, "Units are not relative.");

  template <typename... Args>
  explicit Translation(Args... args) : tensor_(args...) {}
  //  template <typename... Args>
  //  explicit Translation(Args&&... args) : tensor_(std::forward<Args>(args)...) {}

  const TensorT& getTensor() const { return tensor_; }

 private:
  TensorT tensor_;
};

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename From, typename To, typename... Units,
          typename NewCoordinateSystem>
struct CastCoordinateSystem<Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>, NewCoordinateSystem> {
  using type = Translation<ValueMatrix, Dimension, Scalar, NewCoordinateSystem, From, To, Units...>;
};

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename From, typename To, typename... Units,
          typename CoordinateMatrix>
struct TransformationTraits<Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>, Tensor<1, 0, CoordinateMatrix>> {
  static auto transform(const Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Units...>& transformation,
                        const Tensor<1, 0, CoordinateMatrix>& object) {
    if constexpr (IsAbsolute<Tensor<1, 0, CoordinateMatrix>>::value) {
      const auto fromResult = transformation.getTensor() + object;
      const auto toResult =
          typename CastCoordinateSystem<std::decay_t<decltype(fromResult)>, To>::type(fromResult.getCoordinateMatrix().getQuantityMatrix());
      return evaluate(toResult);
    } else {
      const auto fromResult = object;
      const auto toResult =
          typename CastCoordinateSystem<std::decay_t<decltype(fromResult)>, To>::type(fromResult.getCoordinateMatrix().getQuantityMatrix());
      return evaluate(toResult);
    }
  }
};

template <template <typename, int, int> typename ValueMatrix, int Dimension, typename Scalar, typename CoordinateSystem, typename From, typename To>
using SpatialTranslation = Translation<ValueMatrix, Dimension, Scalar, CoordinateSystem, From, To, Length>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To>
using SpatialTranslation2D = SpatialTranslation<ValueMatrix, 2, Scalar, CoordinateSystem, From, To>;
template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To>
using SpatialTranslation3D = SpatialTranslation<ValueMatrix, 3, Scalar, CoordinateSystem, From, To>;

}  // namespace wuzp
