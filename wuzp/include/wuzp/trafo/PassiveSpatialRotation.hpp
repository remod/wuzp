#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformationTemplate.hpp"
#include "wuzp/trafo/PassiveTransformation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"
#include "wuzp/trafo/internal/rigid.hpp"
#include "wuzp/trafo/internal/rotation.hpp"
#include "wuzp/trafo/internal/validity.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

/**
 * A passive rotation from the coordinate system From to the coordinate system To, which have the same origin: the dimensionless
 * passive linear transformation whose matrix R, which transforms the components v of a vector in From into its components R v in
 * To, is orthogonal with determinant 1, R R^T = I and det(R) = 1. Orthogonality is defined by the Euclidean metric, so From and
 * To must be Euclidean (EuclideanCoordinateSystem, see Metric); between Minkowski coordinate systems, a spatial rotation is a
 * LorentzTransformation. Its rows are the basis vectors of To, expressed in From. It transforms the same tensors as the passive
 * linear transformation with its matrix (see PassiveLinearTransformation). It composes with rotations into rotations, with linear
 * transformations into linear transformations, with Jacobians into Jacobians, and with translations and proper rigid
 * transformations into proper rigid transformations (see CompositionTraits). Its inverse, from To to From, is its transpose,
 * which every value matrix backend provides, also for integer matrices, and with which it transforms covectors and tensors with
 * lower indices.
 * In debug builds, i.e. if NDEBUG is not defined, the constructors check that the matrix is a rotation (see isValid()), like
 * Drake's RotationMatrix (https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rotation_matrix.html): they throw
 * std::invalid_argument if it is not, e.g. for a shear, a scaling or a reflection. Release builds do not check. makeUnchecked()
 * skips the check, e.g. in hot paths, and projectToRotation() computes the rotation nearest to a matrix, e.g. to one whose
 * rounding errors accumulated.
 * TODO(remod): Call "unitless rotation" or simply "rotation"?
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_, EuclideanCoordinateSystem FromT_,
          EuclideanCoordinateSystem ToT_, int DimensionV_>
  requires AnyValueMatrix<ValuesT_>
class PassiveSpatialRotation
    : public PassiveTransformation<PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>, FromT_, ToT_> {
 public:
  //! The passive linear transformation with the matrix of the rotation, whose transformations the rotation shares.
  using LinearTransformationT = PassiveLinearTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_,
                                                            typename AccessUnits<AccessImplementation<ValueT_>>::Dimensionless>;
  using ValueT = ValueT_;
  using ValuesT = typename LinearTransformationT::ValuesT;
  using UnitsT = typename LinearTransformationT::UnitsT;
  using QuantitiesT = typename LinearTransformationT::QuantitiesT;

  //! The identity: From and To have the same axes.
  PassiveSpatialRotation() : quantities_{internal::identity<ValuesT>()} {}

  //! The rotation whose matrix is constructed from the arguments, e.g. its elements row by row, or its quantities. Copies and moves
  //! are left to the implicit constructors, see CopiesOrMoves.
  //! @throws std::invalid_argument In debug builds, if the matrix is not a rotation (see isValid()).
  template <typename... Args>
    requires(sizeof...(Args) > 0 && !CopiesOrMoves<PassiveSpatialRotation, Args...>)
  explicit PassiveSpatialRotation(Args&&... args) : quantities_{std::forward<Args>(args)...} {
    internal::checkValidity(*this, "The matrix of a rotation must be orthogonal with determinant 1.");
  }

  //! The rotation whose matrix is constructed from the arguments, without checking it, like Drake's
  //! RotationMatrix::MakeUnchecked(): the caller guarantees that the matrix is a rotation, e.g. a product of rotations.
  template <typename... Args>
  static PassiveSpatialRotation makeUnchecked(Args&&... args) {
    return PassiveSpatialRotation{Unchecked{}, QuantitiesT{std::forward<Args>(args)...}};
  }

  //! The rotation nearest to the matrix constructed from the arguments, e.g. to a rotation whose rounding errors accumulated, by
  //! the Newton-Schulz iteration (see internal::projectToRotation()).
  //! @throws std::invalid_argument In every build, if the matrix A is not close enough to a rotation, i.e. ||I - A^T A|| is not
  //! smaller than 1 in the Frobenius norm, or if its determinant is not positive.
  template <typename... Args>
  static PassiveSpatialRotation projectToRotation(Args&&... args) {
    return makeUnchecked(internal::projectToRotation(QuantitiesT{std::forward<Args>(args)...}.getValues()));
  }

  //! The matrix R, which transforms the components v of a vector in From into its components R v in To.
  const QuantitiesT& getQuantities() const { return quantities_; }

  //! The inverse rotation, from To to From, whose matrix is the transpose R^T = R^-1: it needs no matrix inverse.
  auto inverse() const { return internal::WithFrames<PassiveSpatialRotation, ToT_, FromT_>::type::makeUnchecked(quantities_.transpose()); }

  //! Whether the matrix is a rotation: max_ij |(R R^T - I)_ij| <= tolerance, and det(R) > 0 (see internal::isRotation()). The
  //! constructors check it in debug builds, with the tolerance ValidityTolerance.
  bool isValid(const ValueT tolerance = ValidityTolerance<ValueT>::value()) const {
    return internal::isRotation(quantities_.getValues(), tolerance);
  }

  //! Whether both rotations, which must transform between the same coordinate systems, are approximately equal: their matrices
  //! are compared like the components of tensors (see Tensor::isApprox()).
  template <template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_, typename FromOtherT_, typename ToOtherT_,
            int DimensionOtherV_>
  bool isApprox(const PassiveSpatialRotation<ValuesOtherT_, ValueOtherT_, FromOtherT_, ToOtherT_, DimensionOtherV_>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    static_assert(internal::SameFrames<PassiveSpatialRotation, std::remove_cvref_t<decltype(other)>>::value);
    return quantities_.isApprox(other.getQuantities(), precision);
  }

 private:
  struct Unchecked {};
  PassiveSpatialRotation(Unchecked /*tag*/, QuantitiesT quantities) : quantities_{std::move(quantities)} {}

  //! The matrix R, which transforms the components v of a vector in From into its components R v in To.
  QuantitiesT quantities_;
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_>
using PassiveSpatialRotation2 = PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, ToT_, 2>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_>
using PassiveSpatialRotation3 = PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, ToT_, 3>;

namespace internal {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionV_>
struct IsRotation<PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>> : std::true_type {};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionV_>
struct IsChangeOfBasis<PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>> : std::true_type {};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionV_,
          typename NewFromT_, typename NewToT_>
struct WithFrames<PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>, NewFromT_, NewToT_> {
  using type = PassiveSpatialRotation<ValuesT_, ValueT_, NewFromT_, NewToT_, DimensionV_>;
};

}  // namespace internal

// A rotation transforms tensors like the passive linear transformation with its matrix, but with its transpose as the inverse,
// e.g. for covectors.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionV_,
          typename Object>
struct TransformationTraits<PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>, Object>
    : TransformationTraits<typename PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>::LinearTransformationT, Object> {};

// The composition X_AB * X_BC = X_AC of two rotations is the rotation R_AB R_BC.
template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionRhsV_>
struct CompositionTraits<PassiveSpatialRotation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>,
                         PassiveSpatialRotation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>> {
  using LhsT = PassiveSpatialRotation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>;
  static auto compose(const LhsT& lhs, const PassiveSpatialRotation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>& rhs) {
    return internal::WithFrames<LhsT, FromRhsT_, ToLhsT_>::type::makeUnchecked(innerProduct(lhs.getQuantities(), rhs.getQuantities()));
  }
};

// A rotation is a linear transformation: its compositions with linear transformations are those of the linear transformation with
// its matrix, the products of the matrices.
template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionRhsV_,
          typename... UnitsRhsT_>
struct CompositionTraits<PassiveSpatialRotation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>,
                         PassiveLinearTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_, UnitsRhsT_...>>
    : CompositionTraits<typename PassiveSpatialRotation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>::LinearTransformationT,
                        PassiveLinearTransformation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_, UnitsRhsT_...>> {};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_, int DimensionLhsV_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_,
          typename ToRhsT_, int DimensionRhsV_>
struct CompositionTraits<PassiveLinearTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_, UnitsLhsT_...>,
                         PassiveSpatialRotation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>>
    : CompositionTraits<
          PassiveLinearTransformation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_, UnitsLhsT_...>,
          typename PassiveSpatialRotation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>::LinearTransformationT> {};

// The compositions X_AB * X_BC = X_AC of a rotation with a translation or a proper rigid transformation are proper rigid
// transformations, with a rotation and a translation of the kinds of the operands, or of those of the proper rigid operand (see
// internal/rigid.hpp).

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, int DimensionRhsV_, typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_,
          typename... UnitsRhsT_>
struct CompositionTraits<PassiveSpatialRotation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>,
                         PassiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, FromRhsT_, ToRhsT_, UnitsRhsT_...>> {
  using LhsT = PassiveSpatialRotation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>;
  using RhsT = PassiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, FromRhsT_, ToRhsT_, UnitsRhsT_...>;
  static auto compose(const LhsT& lhs, const RhsT& rhs) {
    return internal::composeRotationAndTranslation<internal::PassiveRigidOf<LhsT, RhsT, FromRhsT_, ToLhsT_>>(lhs, rhs);
  }
};

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_,
          typename ToRhsT_, int DimensionRhsV_>
struct CompositionTraits<PassiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, FromLhsT_, ToLhsT_, UnitsLhsT_...>,
                         PassiveSpatialRotation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>> {
  using LhsT = PassiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, FromLhsT_, ToLhsT_, UnitsLhsT_...>;
  using RhsT = PassiveSpatialRotation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>;
  static auto compose(const LhsT& lhs, const RhsT& rhs) {
    return internal::composeTranslationAndRotation<internal::PassiveRigidOf<RhsT, LhsT, FromRhsT_, ToLhsT_>>(lhs, rhs);
  }
};

template <typename FromLhsT_, typename ToLhsT_, typename RotationLhsT_, typename TranslationLhsT_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionRhsV_>
struct CompositionTraits<PassiveProperRigidTransformationTemplate<FromLhsT_, ToLhsT_, RotationLhsT_, TranslationLhsT_>,
                         PassiveSpatialRotation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>> {
  using LhsT = PassiveProperRigidTransformationTemplate<FromLhsT_, ToLhsT_, RotationLhsT_, TranslationLhsT_>;
  static auto compose(const LhsT& lhs, const PassiveSpatialRotation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>& rhs) {
    return internal::composeRigidAndRotation<internal::PassiveRigidWithFrames<LhsT, FromRhsT_, ToLhsT_>>(lhs, rhs);
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_, int DimensionLhsV_,
          typename FromRhsT_, typename ToRhsT_, typename RotationRhsT_, typename TranslationRhsT_>
struct CompositionTraits<PassiveSpatialRotation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>,
                         PassiveProperRigidTransformationTemplate<FromRhsT_, ToRhsT_, RotationRhsT_, TranslationRhsT_>> {
  using RhsT = PassiveProperRigidTransformationTemplate<FromRhsT_, ToRhsT_, RotationRhsT_, TranslationRhsT_>;
  static auto compose(const PassiveSpatialRotation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>& lhs, const RhsT& rhs) {
    return internal::composeRotationAndRigid<internal::PassiveRigidWithFrames<RhsT, FromRhsT_, ToLhsT_>>(lhs, rhs);
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionRhsV_>
bool operator==(const PassiveSpatialRotation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>& lhs,
                const PassiveSpatialRotation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>& rhs) {
  static_assert(internal::SameFrames<std::remove_cvref_t<decltype(lhs)>, std::remove_cvref_t<decltype(rhs)>>::value);
  return lhs.getQuantities() == rhs.getQuantities();
}

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionRhsV_>
bool operator!=(const PassiveSpatialRotation<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionLhsV_>& lhs,
                const PassiveSpatialRotation<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionRhsV_>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionV_>
std::ostream& operator<<(std::ostream& lhs, const PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>& rhs) {
  lhs << rhs.getQuantities();
  return lhs;
}

// template <typename Derived, typename From, typename To, typename DimensionlessQuantity>
// class QuaternionBase : public PassiveTransformation<Derived, From, To> {
// public:
//  DimensionlessQuantity w() const { return derived().wDerived(); }
//
// private:
//  Derived& derived() { return static_cast<Derived&>(*this); }
//  const Derived& derived() const { return static_cast<const Derived&>(*this); }
//};
//
//// TODO(remod): Do we still need to support quaternions? Or just conversion methods?
// template <typename ValueT, typename From, typename To>
// class Quaternion : public QuaternionBase<Quaternion<ValueT, From, To>, From, To, Quantity<Dimensionless, ValueT>> {
//  // TODO(remod): Using friend is non-extendable. Change this.
//  friend TransformationTraits<Quaternion<ValueT, From, To>, PositionVector<3, ValueT, From>>;
//  friend TransformationTraits<Quaternion<ValueT, From, To>, PositionPoint<3, ValueT, From>>;
//
// public:
//  using DimensionlessQuantity = Quantity<Dimensionless, ValueT>;
//
//  template <typename... Ts>
//  explicit Quaternion(Ts... args) : impl_(args...) {}
//
// protected:
//  DimensionlessQuantity wDerived() const { return impl_.w(); }
//
// private:
//  Eigen::Quaternion<ValueT> impl_;
//};
//
// template <typename From, typename To, typename ValueT>
// struct TransformationTraits<Quaternion<ValueT, From, To>, PositionVector<3, ValueT, From>> {
//  static auto transform(const Quaternion<ValueT, From, To>& transformation, const PositionVector<3, ValueT, From>& object) {
//    return PositionVector<3, ValueT, To>(transformation.impl_ * object.getValues());
//  }
//};
//
// template <typename From, typename To, typename ValueT>
// struct TransformationTraits<Quaternion<ValueT, From, To>, PositionPoint<3, ValueT, From>> {
//  static auto transform(const Quaternion<ValueT, From, To>& transformation, const PositionPoint<3, ValueT, From>& object) {
//    return PositionPoint<3, ValueT, To>(transformation.impl_ * object.getValues());
//  }
//};

}  // namespace wuzp
