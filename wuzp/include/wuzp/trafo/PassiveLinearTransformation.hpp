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
#include "wuzp/trafo/PassiveTransformation.hpp"
#include "wuzp/trafo/internal/apply_units.hpp"
#include "wuzp/trafo/internal/map_relative.hpp"
#include "wuzp/trafo/internal/with_quantities.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, AnyCoordinateSystem FromT_, AnyCoordinateSystem ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_>
  requires AnyValueMatrix<ValuesT_>
class PassiveLinearTransformation;

namespace internal {

//! The passive linear transformation from FromT_ to ToT_ whose matrix has the quantities QuantitiesT_, e.g. the product or the
//! inverse of the matrices of linear transformations, with the units given like the aliases give them (see ApplyUnits), so that
//! e.g. the product of two dimensionless linear transformations has a single dimensionless unit.
template <template <typename, int, int> typename ValuesT_, typename FromT_, typename ToT_, typename QuantitiesT_>
struct PassiveLinearTransformationOf {
  template <typename... UnitsT_>
  using WithUnits = PassiveLinearTransformation<ValuesT_, typename QuantitiesT_::ValueT, FromT_, ToT_, Cols<QuantitiesT_>::value,
                                                Rows<QuantitiesT_>::value, UnitsT_...>;
  using type = typename ApplyUnits<typename QuantitiesT_::UnitsT, WithUnits>::type;
};

template <template <typename, int, int> typename ValuesT_, typename FromT_, typename ToT_, typename QuantitiesT_>
auto makePassiveLinearTransformation(QuantitiesT_ quantities) {
  return typename PassiveLinearTransformationOf<ValuesT_, FromT_, ToT_, QuantitiesT_>::type{std::move(quantities)};
}

}  // namespace internal

/**
 * A passive linear transformation from the coordinate system From to the coordinate system To, possibly of another dimension,
 * whose matrix A transforms the components v of a vector in From into its components A v in To. The objects it transforms must
 * be expressed in From. Rotations, Jacobians and Lorentz transformations are passive linear transformations (see
 * PassiveSpatialRotation, Jacobian and LorentzTransformation): Jacobians are an alias, rotations and Lorentz transformations
 * classes of their own, which check their validity and invert without a matrix inverse.
 * The rows of A are the dual basis covectors of To, and the basis vectors of To are the columns of A^-1, all expressed in From:
 * the dual basis covectors transform like the components, and the basis vectors with the inverse (Carroll, Lecture Notes on
 * General Relativity, https://arxiv.org/abs/gr-qc/9712019, eqs. 1.38 and 1.30). Only if both bases are orthonormal, e.g. for a
 * rotation, whose inverse is its transpose, are the rows of A also the basis vectors of To.
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_, AnyCoordinateSystem FromT_, AnyCoordinateSystem ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_>
  requires AnyValueMatrix<ValuesT_>
class PassiveLinearTransformation
    : public PassiveTransformation<PassiveLinearTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                                   FromT_, ToT_> {
 public:
  using ValueT = ValueT_;
  using ValuesT = ValuesT_<ValueT_, DimensionToV_, DimensionFromV_>;
  using UnitsT = TypeMatrix<DimensionToV_, DimensionFromV_, UnitsT_...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;

  // Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  template <typename... Args>
    requires(!CopiesOrMoves<PassiveLinearTransformation, Args...>)
  explicit PassiveLinearTransformation(Args&&... args) : quantities_{std::forward<Args>(args)...} {}

  //! The matrix A, which transforms the components v of a vector in From into its components A v in To. It maps one coordinate
  //! system into another, so it is not a tensor in one coordinate system, and there is no getTensor(), unlike for active linear
  //! transformations.
  const QuantitiesT& getQuantities() const { return quantities_; }

  /**
   * The inverse transformation, from To to From, whose matrix is the matrix inverse A^-1 (see Quantities::inverse()): A must be
   * square, and its units must describe a linear map. This needs a value matrix backend with matrix inverses
   * (InvertibleValueMatrixBackend). The inverses of rotations and Lorentz transformations need no matrix inverse (see
   * PassiveSpatialRotation::inverse() and LorentzTransformation::inverse()).
   */
  auto inverse() const { return internal::makePassiveLinearTransformation<ValuesT_, ToT_, FromT_>(quantities_.inverse()); }

  //! Whether both linear transformations, which must transform between the same coordinate systems, are approximately equal:
  //! their matrices are compared like the components of tensors (see Tensor::isApprox()), also those that are not square.
  template <template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_, typename FromOtherT_, typename ToOtherT_,
            int DimensionFromOtherV_, int DimensionToOtherV_, typename... UnitsOtherT_>
  bool isApprox(const PassiveLinearTransformation<ValuesOtherT_, ValueOtherT_, FromOtherT_, ToOtherT_, DimensionFromOtherV_,
                                                  DimensionToOtherV_, UnitsOtherT_...>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    static_assert(internal::SameFrames<PassiveLinearTransformation, std::remove_cvref_t<decltype(other)>>::value);
    return quantities_.isApprox(other.getQuantities(), precision);
  }

 private:
  //! The matrix A, which transforms the components v of a vector in From into its components A v in To. It is multiplied with
  //! the quantities of the tensors it transforms (see internal::withQuantities()).
  QuantitiesT quantities_;
};

namespace internal {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename NewFromT_, typename NewToT_>
struct WithFrames<PassiveLinearTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>, NewFromT_,
                  NewToT_> {
  using type = PassiveLinearTransformation<ValuesT_, ValueT_, NewFromT_, NewToT_, DimensionFromV_, DimensionToV_, UnitsT_...>;
};

}  // namespace internal

// The tensors are expressed in From (see PassiveTransformation::transform()). Their components are transformed, and the result
// is expressed in To.
// The transformations of tensors by a passive linear transformation, and its compositions, only use its matrix, getQuantities(),
// and the matrix of its inverse, inverse().getQuantities(): they take any transformation with the matrix of the linear
// transformation, so that those of the rotations and of the Lorentz transformations, which are linear transformations too,
// inherit them (see PassiveSpatialRotation and LorentzTransformation). Their inverses, R^T and eta Lambda^T eta, need no matrix
// inverse, so they transform covectors and tensors with lower indices without its rounding errors, and also for integer
// matrices.

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<PassiveLinearTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  template <typename TransformationT_>
  static auto transform(const TransformationT_& transformation,
                        const Tensor<Indices<Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    // A covector transforms with the inverse: w' = w A^-1.
    return castCoordinateSystem<ToT_>(
        internal::withQuantities<FromT_>(object, innerProduct(object.getQuantities(), transformation.inverse().getQuantities())));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<PassiveLinearTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Down, Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  template <typename TransformationT_>
  static auto transform(const TransformationT_& transformation,
                        const Tensor<Indices<Down, Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    // A tensor with two lower indices, e.g. a mass matrix, transforms like M' = A^-T M A^-1.
    const auto inverse = transformation.inverse().getQuantities();
    return castCoordinateSystem<ToT_>(
        internal::withQuantities<FromT_>(object, innerProduct(inverse.transpose(), innerProduct(object.getQuantities(), inverse))));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_, typename AbsoluteOtherT_>
struct TransformationTraits<PassiveLinearTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  template <typename TransformationT_>
  static auto transform(const TransformationT_& transformation,
                        const Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>& object) {
    // A vector transforms like v' = A v.
    return castCoordinateSystem<ToT_>(internal::mapRelative(object, [&transformation](const auto& relative) {
      return internal::withQuantities<FromT_>(relative, innerProduct(transformation.getQuantities(), relative.getQuantities()));
    }));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<PassiveLinearTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Up, Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  template <typename TransformationT_>
  static auto transform(const TransformationT_& transformation,
                        const Tensor<Indices<Up, Down>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    // A linear map transforms like T' = A T A^-1.
    return castCoordinateSystem<ToT_>(internal::withQuantities<FromT_>(
        object,
        innerProduct(transformation.getQuantities(), innerProduct(object.getQuantities(), transformation.inverse().getQuantities()))));
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<PassiveLinearTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Up, Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  template <typename TransformationT_>
  static auto transform(const TransformationT_& transformation,
                        const Tensor<Indices<Up, Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    // A tensor with two upper indices, e.g. a covariance matrix, transforms like T' = A T A^T.
    return castCoordinateSystem<ToT_>(internal::withQuantities<FromT_>(
        object,
        innerProduct(innerProduct(transformation.getQuantities(), object.getQuantities()), transformation.getQuantities().transpose())));
  }
};

// The composition X_AB * X_BC = X_AC of two linear transformations is the product of their matrices, A_lhs A_rhs, e.g. the
// chain rule for Jacobians.
template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_,
          int DimensionFromLhsV_, int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_,
          typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
struct CompositionTraits<
    PassiveLinearTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionFromLhsV_, DimensionToLhsV_, UnitsLhsT_...>,
    PassiveLinearTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionFromRhsV_, DimensionToRhsV_, UnitsRhsT_...>> {
  template <typename LhsT_, typename RhsT_>
  static auto compose(const LhsT_& lhs, const RhsT_& rhs) {
    return internal::makePassiveLinearTransformation<ValuesLhsT_, FromRhsT_, ToLhsT_>(
        innerProduct(lhs.getQuantities(), rhs.getQuantities()));
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_,
          int DimensionFromLhsV_, int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_,
          typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
bool operator==(const PassiveLinearTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionFromLhsV_, DimensionToLhsV_,
                                                  UnitsLhsT_...>& lhs,
                const PassiveLinearTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionFromRhsV_, DimensionToRhsV_,
                                                  UnitsRhsT_...>& rhs) {
  static_assert(internal::SameFrames<std::remove_cvref_t<decltype(lhs)>, std::remove_cvref_t<decltype(rhs)>>::value);
  return lhs.getQuantities() == rhs.getQuantities();
}

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_,
          int DimensionFromLhsV_, int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_,
          typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
bool operator!=(const PassiveLinearTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionFromLhsV_, DimensionToLhsV_,
                                                  UnitsLhsT_...>& lhs,
                const PassiveLinearTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionFromRhsV_, DimensionToRhsV_,
                                                  UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_>
std::ostream& operator<<(
    std::ostream& lhs,
    const PassiveLinearTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>& rhs) {
  lhs << rhs.getQuantities();
  return lhs;
}

}  // namespace wuzp
