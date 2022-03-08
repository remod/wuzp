#pragma once

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/trafo/Transformation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystemT_, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units>
class PassiveLinearTransformation
    : public PassiveTransformation<
          PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystemT_, From, To, DimensionFrom, DimensionTo, Units...>, From, To> {
 public:
  using ValueMatrixT = ValueMatrix<Scalar, DimensionTo, DimensionFrom>;
  using UnitMatrixT = TypeMatrix<DimensionTo, DimensionFrom, Units...>;
  using QuantityMatrixT = QuantityMatrix<ValueMatrixT, UnitMatrixT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<1, 1, QuantityMatrixT, CoordinateSystemT>;

  template <typename... Args>
  explicit PassiveLinearTransformation(Args... args) : tensor_(args...) {}

  const TensorT& getTensor() const { return tensor_; }

 private:
  //! Tensor holding the orientation (= active rotation) of coordinate system To expressed in coordinate system From.
  TensorT tensor_;
};

}  // namespace wuzp

#include "wuzp/trafo/PassiveTranslation.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units>
struct GetCoordinateSystem<
    PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>> {
  using type = CoordinateSystem;
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename NewCoordinateSystem>
struct CastCoordinateSystem<
    PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
    NewCoordinateSystem> {
  using type = PassiveLinearTransformation<ValueMatrix, Scalar, NewCoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>;
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<
    PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
    Tensor<0, 1, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(const PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo,
                                                          Units...>& transformation,
                        const Tensor<0, 1, QuantityMatrixT_, CoordinateSystemT_>& object) {
    //    return transformation.getTensor() * object;
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<
    PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
    Tensor<0, 2, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(const PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo,
                                                          Units...>& transformation,
                        const Tensor<0, 2, QuantityMatrixT_, CoordinateSystemT_>& object) {
    const auto fromResult = transformation.getTensor().inverse().transpose() * object * transformation.getTensor().inverse();
    return evaluate(castCoordinateSystem<To>(fromResult.getTensor()));
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<
    PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
    Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(const PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo,
                                                          Units...>& transformation,
                        const Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>& object) {
    // If the object is absolute (point), make it relative (vector), transform it, and make it absolute again.
    if constexpr (IsAbsolute<Tensor<1, 0, QuantityMatrixT_, CoordinateSystemT_>>::value) {
      return evaluate(castCoordinateSystem<To>(makeAbsolute(transformation.getTensor() * makeRelative(object))));
    } else {
      return evaluate(castCoordinateSystem<To>(transformation.getTensor() * object));
    }
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<
    PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
    Tensor<1, 1, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(const PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo,
                                                          Units...>& transformation,
                        const Tensor<1, 1, QuantityMatrixT_, CoordinateSystemT_>& object) {
    return castCoordinateSystem<To>(transformation.getTensor() * object * transformation.getTensor().inverse());
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, typename QuantityMatrixT_, typename CoordinateSystemT_>
struct TransformationTraits<
    PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
    Tensor<2, 0, QuantityMatrixT_, CoordinateSystemT_>> {
  static auto transform(const PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo,
                                                          Units...>& transformation,
                        const Tensor<2, 0, QuantityMatrixT_, CoordinateSystemT_>& object) {
    //    return transformation.getTensor() * object;
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, template <typename, int, int> typename ValueMatrixOther,
          int DimensionOther, typename ScalarOther, typename CoordinateSystemOther, typename FromOther, typename ToOther,
          typename... UnitsOther>
struct TransformationTraits<
    PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
    PassiveTranslation<ValueMatrixOther, DimensionOther, ScalarOther, CoordinateSystemOther, FromOther, ToOther, UnitsOther...>> {
  static auto transform(const PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo,
                                                          Units...>& transformation,
                        const PassiveTranslation<ValueMatrixOther, DimensionOther, ScalarOther, CoordinateSystemOther, FromOther, ToOther,
                                                 UnitsOther...>& object) {
    return PassiveTranslation<ValueMatrixOther, DimensionOther, ScalarOther, To, FromOther, ToOther, UnitsOther...>(
        transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units, template <typename, int, int> typename ValueMatrixOther,
          typename ScalarOther, typename CoordinateSystemOther, typename FromOther, typename ToOther, int DimensionFromOther,
          int DimensionToOther, typename... UnitsOther>
struct TransformationTraits<
    PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>,
    PassiveLinearTransformation<ValueMatrixOther, ScalarOther, CoordinateSystemOther, FromOther, ToOther, DimensionFromOther,
                                DimensionToOther, UnitsOther...>> {
  static auto transform(const PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo,
                                                          Units...>& transformation,
                        const PassiveLinearTransformation<ValueMatrixOther, ScalarOther, CoordinateSystemOther, FromOther, ToOther,
                                                          DimensionFromOther, DimensionToOther, UnitsOther...>& object) {
    return PassiveLinearTransformation<ValueMatrixOther, ScalarOther, To, FromOther, ToOther, DimensionFromOther, DimensionToOther,
                                       UnitsOther...>(transformation.transform(object.getTensor()));
  }
};

template <template <typename, int, int> typename ValueMatrixLhs, typename ScalarLhs, typename CoordinateSystemLhs, typename FromLhs,
          typename ToLhs, int DimensionFromLhs, int DimensionToLhs, typename... UnitsLhs,
          template <typename, int, int> typename ValueMatrixRhs, typename ScalarRhs, typename CoordinateSystemRhs, typename FromRhs,
          typename ToRhs, int DimensionFromRhs, int DimensionToRhs, typename... UnitsRhs>
bool operator==(const PassiveLinearTransformation<ValueMatrixLhs, ScalarLhs, CoordinateSystemLhs, FromLhs, ToLhs, DimensionFromLhs,
                                                  DimensionToLhs, UnitsLhs...>& lhs,
                const PassiveLinearTransformation<ValueMatrixRhs, ScalarRhs, CoordinateSystemRhs, FromRhs, ToRhs, DimensionFromRhs,
                                                  DimensionToRhs, UnitsRhs...>& rhs) {
  static_assert(std::is_same<FromLhs, FromRhs>::value, "From coordinates frames do not match.");
  static_assert(std::is_same<ToLhs, ToRhs>::value, "To coordinates frames do not match.");
  return lhs.getTensor() == rhs.getTensor();
}

template <template <typename, int, int> typename ValueMatrixLhs, typename ScalarLhs, typename CoordinateSystemLhs, typename FromLhs,
          typename ToLhs, int DimensionFromLhs, int DimensionToLhs, typename... UnitsLhs,
          template <typename, int, int> typename ValueMatrixRhs, typename ScalarRhs, typename CoordinateSystemRhs, typename FromRhs,
          typename ToRhs, int DimensionFromRhs, int DimensionToRhs, typename... UnitsRhs>
bool operator!=(const PassiveLinearTransformation<ValueMatrixLhs, ScalarLhs, CoordinateSystemLhs, FromLhs, ToLhs, DimensionFromLhs,
                                                  DimensionToLhs, UnitsLhs...>& lhs,
                const PassiveLinearTransformation<ValueMatrixRhs, ScalarRhs, CoordinateSystemRhs, FromRhs, ToRhs, DimensionFromRhs,
                                                  DimensionToRhs, UnitsRhs...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValueMatrix, typename Scalar, typename CoordinateSystem, typename From, typename To,
          int DimensionFrom, int DimensionTo, typename... Units>
std::ostream& operator<<(
    std::ostream& stream,
    const PassiveLinearTransformation<ValueMatrix, Scalar, CoordinateSystem, From, To, DimensionFrom, DimensionTo, Units...>& translation) {
  stream << translation.getTensor();
  return stream;
}

}  // namespace wuzp
