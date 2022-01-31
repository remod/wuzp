#pragma once

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units>
class LinearTransformation
    : public PassiveTransformation<
          LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>, CoordinateSystem,
          From, To> {
 public:
  using ValueMatrixT = ValueMatrix<Scalar, DimensionTo, DimensionFrom>;
  using UnitMatrixT = TypeMatrix<DimensionTo, DimensionFrom, Units...>;
  using QuantityMatrixT = QuantityMatrix<ValueMatrixT, UnitMatrixT>;
  using CoordinateSystemMatrixT = TypeMatrix<DimensionTo, DimensionFrom, CoordinateSystem>;
  using CoordinateMatrixT = CoordinateMatrix<QuantityMatrixT, CoordinateSystemMatrixT>;
  using TensorT = Tensor<1, 1, CoordinateMatrixT>;

  template <typename... Args>
  explicit LinearTransformation(Args... args) : tensor_(args...) {}

  const TensorT& getTensor() const { return tensor_; }

 private:
  TensorT tensor_;
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units>
struct GetCoordinateSystem<LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>> {
  using type = CoordinateSystem;
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename NewCoordinateSystem>
struct CastCoordinateSystem<LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            NewCoordinateSystem> {
  using type = LinearTransformation<ValueMatrix, Scalar, NewCoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>;
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename CoordinateMatrix>
struct TransformationTraits<LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<0, 1, CoordinateMatrix>> {
  static auto transform(
      const LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<0, 1, CoordinateMatrix>& object) {
    //    return transformation.getTensor() * object;
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename CoordinateMatrix>
struct TransformationTraits<LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<0, 2, CoordinateMatrix>> {
  static auto transform(
      const LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<0, 2, CoordinateMatrix>& object) {
    const auto fromResult = transformation.getTensor().inverse().transpose() * object * transformation.getTensor().inverse();
    return evaluate(castCoordinateSystem<To>(fromResult.getTensor()));
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename CoordinateMatrix>
struct TransformationTraits<LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<1, 0, CoordinateMatrix>> {
  static auto transform(
      const LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<1, 0, CoordinateMatrix>& object) {
    // If the object is absolute (point), make it relative (vector), transform it, and make it absolute again.
    if constexpr (IsAbsolute<Tensor<1, 0, CoordinateMatrix>>::value) {
      return evaluate(castCoordinateSystem<To>(makeAbsolute(transformation.getTensor() * makeRelative(object))));
    } else {
      return evaluate(castCoordinateSystem<To>(transformation.getTensor() * object));
    }
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename CoordinateMatrix>
struct TransformationTraits<LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<1, 1, CoordinateMatrix>> {
  static auto transform(
      const LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<1, 1, CoordinateMatrix>& object) {
    return transformation.getTensor() * object * transformation.getTensor().inverse();
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename CoordinateMatrix>
struct TransformationTraits<LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
                            Tensor<2, 0, CoordinateMatrix>> {
  static auto transform(
      const LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& transformation,
      const Tensor<2, 0, CoordinateMatrix>& object) {
    //    return transformation.getTensor() * object;
  }
};

template <template <typename, int, int> typename ValueMatrixLhs, typename ScalarLhs, typename CoordinateSystemLhs, typename FromLhs,
          typename ToLhs, int DimensionFromLhs, int DimensionToLhs, typename... UnitsLhs,
          template <typename, int, int> typename ValueMatrixRhs, typename ScalarRhs, typename CoordinateSystemRhs, typename FromRhs,
          typename ToRhs, int DimensionFromRhs, int DimensionToRhs, typename... UnitsRhs>
bool operator==(const LinearTransformation<ValueMatrixLhs, ScalarLhs, CoordinateSystemLhs, FromLhs, ToLhs, DimensionFromLhs, DimensionToLhs,
                                           UnitsLhs...>& lhs,
                const LinearTransformation<ValueMatrixRhs, ScalarRhs, CoordinateSystemRhs, FromRhs, ToRhs, DimensionFromRhs, DimensionToRhs,
                                           UnitsRhs...>& rhs) {
  static_assert(std::is_same<FromLhs, FromRhs>::value, "From coordinates frames do not match.");
  static_assert(std::is_same<ToLhs, ToRhs>::value, "To coordinates frames do not match.");
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValueMatrixLhs, typename ScalarLhs, typename CoordinateSystemLhs, typename FromLhs,
          typename ToLhs, int DimensionFromLhs, int DimensionToLhs, typename... UnitsLhs,
          template <typename, int, int> typename ValueMatrixRhs, typename ScalarRhs, typename CoordinateSystemRhs, typename FromRhs,
          typename ToRhs, int DimensionFromRhs, int DimensionToRhs, typename... UnitsRhs>
bool operator!=(const LinearTransformation<ValueMatrixLhs, ScalarLhs, CoordinateSystemLhs, FromLhs, ToLhs, DimensionFromLhs, DimensionToLhs,
                                           UnitsLhs...>& lhs,
                const LinearTransformation<ValueMatrixRhs, ScalarRhs, CoordinateSystemRhs, FromRhs, ToRhs, DimensionFromRhs, DimensionToRhs,
                                           UnitsRhs...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units>
std::ostream& operator<<(
    std::ostream& stream,
    const LinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& translation) {
  stream << translation.getTensor();
  return stream;
}

}  // namespace wuzp
