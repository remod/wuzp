#pragma once

#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

template <int Dimension, typename Scalar, typename CoordinateSystem, typename From, typename To, typename... Units>
class Translation : public PassiveTransformation<Translation<Dimension, Scalar, CoordinateSystem, From, To, Units...>, From, To> {
 public:
  using ValueMatrixT = ValueMatrix<Scalar, Dimension, 1>;
  using UnitMatrixT = UnitMatrix<Dimension, 1, Units...>;
  using CoordinateSystemMatrixT = CoordinateSystemMatrix<Dimension, 1, CoordinateSystem>;
  using CoordinateMatrixT = CoordinateMatrix<ValueMatrixT, UnitMatrixT, CoordinateSystemMatrixT>;
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

template <int Dimension, typename Scalar, typename CoordinateSystem, typename From, typename To, typename... Units,
          typename NewCoordinateSystem>
struct CastCoordinateSystem<Translation<Dimension, Scalar, CoordinateSystem, From, To, Units...>, NewCoordinateSystem> {
  using type = Translation<Dimension, Scalar, NewCoordinateSystem, From, To, Units...>;
};

template <int Dimension, typename Scalar, typename CoordinateSystem, typename From, typename To, typename... Units,
          typename CoordinateMatrix>
struct TransformationTraits<Translation<Dimension, Scalar, CoordinateSystem, From, To, Units...>, Tensor<1, 0, CoordinateMatrix>> {
  static auto transform(const Translation<Dimension, Scalar, CoordinateSystem, From, To, Units...>& transformation,
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

template <int Dimension, typename Scalar, typename CoordinateSystem, typename From, typename To>
using SpatialTranslation = Translation<Dimension, Scalar, CoordinateSystem, From, To, Length>;
template <typename Scalar, typename CoordinateSystem, typename From, typename To>
using SpatialTranslation2D = SpatialTranslation<2, Scalar, CoordinateSystem, From, To>;
template <typename Scalar, typename CoordinateSystem, typename From, typename To>
using SpatialTranslation3D = SpatialTranslation<3, Scalar, CoordinateSystem, From, To>;

}  // namespace wuzp
