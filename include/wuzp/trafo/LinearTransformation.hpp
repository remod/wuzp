#pragma once

#include "wuzp/core/Tensor.hpp"
#include "wuzp/trafo/Transformation.hpp"
#include "wuzp/units/Quantity.hpp"

namespace wuzp {

template <typename Scalar, typename CoordinateSystem, typename From, typename To, int DimensionFrom, int DimensionTo, typename... Units>
class LinearTransformation
    : public PassiveTransformation<LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>, From,
                                   To> {
 public:
  using ValueMatrixT = ValueMatrix<Scalar, DimensionTo, DimensionFrom>;
  using UnitMatrixT = UnitMatrix<DimensionTo, DimensionFrom, Units...>;
  using CoordinateSystemMatrixT = CoordinateSystemMatrix<DimensionTo, DimensionFrom, CoordinateSystem>;
  using CoordinateMatrixT = CoordinateMatrix<ValueMatrixT, UnitMatrixT, CoordinateSystemMatrixT>;
  using TensorT = Tensor<1, 1, CoordinateMatrixT>;

  template <typename... Args>
  explicit LinearTransformation(Args... args) : tensor_(args...) {}

  const TensorT& getTensor() const { return tensor_; }

 private:
  TensorT tensor_;
};

template <typename Scalar, typename CoordinateSystem, typename From, typename To, int DimensionFrom, int DimensionTo, typename... Units,
          typename CoordinateMatrix>
struct TransformationTraits<LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<0, 1, CoordinateMatrix>> {
  static auto transform(
      const LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<0, 1, CoordinateMatrix>& object) {
    //    return transformation.getTensor() * object;
  }
};
template <typename Scalar, typename CoordinateSystem, typename From, typename To, int DimensionFrom, int DimensionTo, typename... Units,
          typename CoordinateMatrix>
struct TransformationTraits<LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<0, 2, CoordinateMatrix>> {
  static auto transform(
      const LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<0, 2, CoordinateMatrix>& object) {
    const auto fromResult = transformation.getTensor().inverse().transpose() * object * transformation.getTensor().inverse();
    return typename CastCoordinateSystem<std::decay_t<decltype(fromResult)>, To>::type(
        fromResult.getTensor().getCoordinateMatrix().getQuantityMatrix());
  }
};
template <typename T>
auto makeRelative(T t) {
  return MakeRelative<T>::make(t);
}
template <bool Enable, typename T>
auto makeAbsolute(T t) {
  return MakeAbsolute<T, Enable>::make(t);
}
template <typename To, typename T>
auto castCoordinateSystem(T t) {
  return typename CastCoordinateSystem<T, To>::type(t.getCoordinateMatrix().getQuantityMatrix());
}
template <typename T>
auto evaluate(T t) {
  return typename Evaluate<T>::type(t);
}
template <typename Scalar, typename CoordinateSystem, typename From, typename To, int DimensionFrom, int DimensionTo, typename... Units,
          typename CoordinateMatrix>
struct TransformationTraits<LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<1, 0, CoordinateMatrix>> {
  static auto transform(
      const LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<1, 0, CoordinateMatrix>& object) {
    // If the object is absolute (point), make it relative (vector), transform it, and make it absolute again.
    return evaluate(castCoordinateSystem<To>(
        makeAbsolute<IsAbsolute<Tensor<1, 0, CoordinateMatrix>>::value>(transformation.getTensor() * makeRelative(object))));
  }
};
template <typename Scalar, typename CoordinateSystem, typename From, typename To, int DimensionFrom, int DimensionTo, typename... Units,
          typename CoordinateMatrix>
struct TransformationTraits<LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<1, 1, CoordinateMatrix>> {
  static auto transform(
      const LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<1, 1, CoordinateMatrix>& object) {
    return transformation.getTensor() * object * transformation.getTensor().inverse();
  }
};
template <typename Scalar, typename CoordinateSystem, typename From, typename To, int DimensionFrom, int DimensionTo, typename... Units,
          typename CoordinateMatrix>
struct TransformationTraits<LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<2, 0, CoordinateMatrix>> {
  static auto transform(
      const LinearTransformation<Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<2, 0, CoordinateMatrix>& object) {
    //    return transformation.getTensor() * object;
  }
};

}  // namespace wuzp
