#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveTransformation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"
#include "wuzp/trafo/internal/apply_units.hpp"
#include "wuzp/trafo/internal/pull_back.hpp"
#include "wuzp/trafo/internal/push_forward.hpp"
#include "wuzp/trafo/internal/with_quantities.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, AnyCoordinateSystem CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_>
  requires AnyValueMatrix<ValuesT_>
class ActiveLinearTransformation;

namespace internal {

//! The active linear transformation in CoordinateSystemT_ whose matrix has the quantities QuantitiesT_, e.g. the product or the
//! inverse of the matrices of linear transformations, with the units given like the aliases give them (see ApplyUnits), so that
//! e.g. the product of two dimensionless linear transformations has a single dimensionless unit.
template <template <typename, int, int> typename ValuesT_, typename CoordinateSystemT_, typename QuantitiesT_>
struct ActiveLinearTransformationOf {
  template <typename... UnitsT_>
  using WithUnits = ActiveLinearTransformation<ValuesT_, typename QuantitiesT_::ValueT, CoordinateSystemT_, Cols<QuantitiesT_>::value,
                                               Rows<QuantitiesT_>::value, UnitsT_...>;
  using type = typename ApplyUnits<typename QuantitiesT_::UnitsT, WithUnits>::type;
};

template <template <typename, int, int> typename ValuesT_, typename CoordinateSystemT_, typename QuantitiesT_>
auto makeActiveLinearTransformation(QuantitiesT_ quantities) {
  return typename ActiveLinearTransformationOf<ValuesT_, CoordinateSystemT_, QuantitiesT_>::type{std::move(quantities)};
}

}  // namespace internal

template <template <typename, int, int> typename ValuesT_, typename ValueT_, AnyCoordinateSystem CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_>
  requires AnyValueMatrix<ValuesT_>
class ActiveLinearTransformation
    : public ActiveTransformation<
          ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>> {
 public:
  using ValueT = ValueT_;
  using ValuesT = ValuesT_<ValueT_, DimensionToV_, DimensionFromV_>;
  using UnitsT = TypeMatrix<DimensionToV_, DimensionFromV_, UnitsT_...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = Tensor<Indices<Up, Down>, QuantitiesT, CoordinateSystemT, Relative>;

  // Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  template <typename... Args>
    requires(!CopiesOrMoves<ActiveLinearTransformation, Args...>)
  explicit ActiveLinearTransformation(Args&&... args) : quantities_{std::forward<Args>(args)...} {}

  //! The matrix A, which transforms a vector v into A v.
  const QuantitiesT& getQuantities() const { return quantities_; }
  //! The matrix as a linear map, a tensor with an upper and a lower index; only for a square matrix.
  TensorT getTensor() const
    requires(DimensionFromV_ == DimensionToV_)
  {
    return TensorT{quantities_};
  }

  /**
   * The inverse transformation, whose matrix is the matrix inverse A^-1 (see Quantities::inverse()): A must be square, and its
   * units must describe a linear map. This needs a value matrix backend with matrix inverses (InvertibleValueMatrixBackend).
   * The inverse of a rotation is its transpose (see ActiveSpatialRotation::inverse()).
   */
  auto inverse() const { return internal::makeActiveLinearTransformation<ValuesT_, CoordinateSystemT_>(quantities_.inverse()); }

  //! Whether both linear transformations, which must be expressed in the same coordinate system, are approximately equal: their
  //! matrices are compared like the components of tensors (see Tensor::isApprox()), also those that are not square.
  template <template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_, typename CoordinateSystemOtherT_,
            int DimensionFromOtherV_, int DimensionToOtherV_, typename... UnitsOtherT_>
  bool isApprox(const ActiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionFromOtherV_,
                                                 DimensionToOtherV_, UnitsOtherT_...>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    static_assert(std::is_same_v<CoordinateSystemT_, CoordinateSystemOtherT_>, "Coordinate systems do not match.");
    return quantities_.isApprox(other.getQuantities(), precision);
  }

 private:
  //! The matrix A, which transforms a vector v into A v. It is multiplied with the quantities of the tensors it transforms (see
  //! internal::withQuantities()).
  QuantitiesT quantities_;
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_>
struct GetCoordinateSystem<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>> {
  using type = CoordinateSystemT_;
};

// The transformations of tensors and the conjugations by an active linear transformation, and its compositions, only use its
// matrix, getQuantities(), and the matrix of its inverse, inverse().getQuantities(): they take any transformation with the matrix
// of the linear transformation, so that those of the rotations, which are linear transformations too, inherit them (see
// ActiveSpatialRotation). The inverse of a rotation is its transpose, so a rotation transforms covectors and tensors with lower
// indices without a matrix inverse: without its rounding errors, and also for integer matrices.
// Like a passive linear transformation, it pushes vectors and tensors with two upper indices forward by its matrix, and pulls
// covectors and tensors with two lower indices back by its inverse (see internal::pushForward() and internal::pullBack()), within
// its coordinate system.

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  template <typename TransformationT_>
  static auto transform(const TransformationT_& transformation,
                        const Tensor<Indices<Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    // A covector transforms with the inverse, w' = w A^-1: it is pulled back by A^-1.
    return internal::pullBack<CoordinateSystemT_, CoordinateSystemT_>(transformation.inverse().getQuantities(), object);
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Down, Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  template <typename TransformationT_>
  static auto transform(const TransformationT_& transformation,
                        const Tensor<Indices<Down, Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    // A tensor with two lower indices, e.g. a mass matrix, transforms like M' = A^-T M A^-1: it is pulled back by A^-1.
    return internal::pullBack<CoordinateSystemT_, CoordinateSystemT_>(transformation.inverse().getQuantities(), object);
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  template <typename TransformationT_>
  static auto transform(const TransformationT_& transformation,
                        const Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    // A vector transforms like v' = A v, also a point, through its coordinates relative to the origin.
    return internal::pushForward<CoordinateSystemT_, CoordinateSystemT_>(transformation.getQuantities(), object);
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Up, Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  template <typename TransformationT_>
  static auto transform(const TransformationT_& transformation,
                        const Tensor<Indices<Up, Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    // A linear map transforms like T' = A T A^-1.
    return internal::withQuantities<CoordinateSystemT_>(
        object,
        innerProduct(transformation.getQuantities(), innerProduct(object.getQuantities(), transformation.inverse().getQuantities())));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Up, Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  template <typename TransformationT_>
  static auto transform(const TransformationT_& transformation,
                        const Tensor<Indices<Up, Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    // A tensor with two upper indices, e.g. a covariance matrix, transforms like T' = A T A^T.
    return internal::pushForward<CoordinateSystemT_, CoordinateSystemT_>(transformation.getQuantities(), object);
  }
};

// The conjugation A T_t A^-1 of a translation by a linear transformation is the translation by A t: A (A^-1 x + t) = x + A t.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, template <typename, int, int> typename ValuesOtherT_, int DimensionOtherV_,
          typename ValueOtherT_, typename CoordinateSystemOtherT_, typename... UnitsOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>> {
  template <typename TransformationT_>
  static auto transform(
      const TransformationT_& transformation,
      const ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>& object) {
    return ActiveTranslation<ValuesOtherT_, DimensionOtherV_, ValueOtherT_, CoordinateSystemOtherT_, UnitsOtherT_...>(
        transformation.transform(object.getTensor()));
  }
};

// The conjugation A B A^-1 of a linear transformation by a linear transformation, like the transformation of a linear map.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_,
          typename CoordinateSystemOtherT_, int DimensionFromOtherV_, int DimensionToOtherV_, typename... UnitsOtherT_>
struct TransformationTraits<ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            ActiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionFromOtherV_,
                                                       DimensionToOtherV_, UnitsOtherT_...>> {
  template <typename TransformationT_>
  static auto transform(const TransformationT_& transformation,
                        const ActiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionFromOtherV_,
                                                         DimensionToOtherV_, UnitsOtherT_...>& object) {
    return ActiveLinearTransformation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionFromOtherV_, DimensionToOtherV_,
                                      UnitsOtherT_...>(transformation.transform(object.getTensor()).getQuantities());
  }
};

// The composition of two linear transformations is the product of their matrices, A_lhs A_rhs.
template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionFromLhsV_,
          int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
struct CompositionTraits<
    ActiveLinearTransformation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionFromLhsV_, DimensionToLhsV_, UnitsLhsT_...>,
    ActiveLinearTransformation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionFromRhsV_, DimensionToRhsV_, UnitsRhsT_...>> {
  template <typename LhsT_, typename RhsT_>
  static auto compose(const LhsT_& lhs, const RhsT_& rhs) {
    return internal::makeActiveLinearTransformation<ValuesLhsT_, CoordinateSystemLhsT_>(
        innerProduct(lhs.getQuantities(), rhs.getQuantities()));
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionFromLhsV_,
          int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
bool operator==(const ActiveLinearTransformation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionFromLhsV_, DimensionToLhsV_,
                                                 UnitsLhsT_...>& lhs,
                const ActiveLinearTransformation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionFromRhsV_, DimensionToRhsV_,
                                                 UnitsRhsT_...>& rhs) {
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  return lhs.getQuantities() == rhs.getQuantities();
}

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionFromLhsV_,
          int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
bool operator!=(const ActiveLinearTransformation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionFromLhsV_, DimensionToLhsV_,
                                                 UnitsLhsT_...>& lhs,
                const ActiveLinearTransformation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionFromRhsV_, DimensionToRhsV_,
                                                 UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_>
std::ostream& operator<<(
    std::ostream& lhs,
    const ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionFromV_, DimensionToV_, UnitsT_...>& rhs) {
  lhs << rhs.getQuantities();
  return lhs;
}

}  // namespace wuzp
