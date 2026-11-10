#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveTransformation.hpp"
#include "wuzp/trafo/internal/apply_units.hpp"
#include "wuzp/trafo/internal/pull_back.hpp"
#include "wuzp/trafo/internal/push_forward.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, AnyCoordinateSystem FromT_, AnyCoordinateSystem ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_>
  requires AnyValueMatrix<ValuesT_>
class Jacobian;

namespace internal {

//! The Jacobian from FromT_ to ToT_ whose matrix has the quantities QuantitiesT_, e.g. the product or the inverse of Jacobians,
//! with the units given like the aliases give them (see ApplyUnits and PassiveLinearTransformationOf).
template <template <typename, int, int> typename ValuesT_, typename FromT_, typename ToT_, typename QuantitiesT_>
struct JacobianOf {
  template <typename... UnitsT_>
  using WithUnits =
      Jacobian<ValuesT_, typename QuantitiesT_::ValueT, FromT_, ToT_, Cols<QuantitiesT_>::value, Rows<QuantitiesT_>::value, UnitsT_...>;
  using type = typename ApplyUnits<typename QuantitiesT_::UnitsT, WithUnits>::type;
};

template <template <typename, int, int> typename ValuesT_, typename FromT_, typename ToT_, typename QuantitiesT_>
auto makeJacobian(QuantitiesT_ quantities) {
  return typename JacobianOf<ValuesT_, FromT_, ToT_, QuantitiesT_>::type{std::move(quantities)};
}

//! The composition lhs * rhs of a Jacobian with a Jacobian or a change of basis, by the chain rule J_AC = J_AB J_BC: the Jacobian
//! from From of rhs to To of lhs whose matrix is the product of their matrices.
template <template <typename, int, int> typename ValuesT_, typename LhsT_, typename RhsT_>
auto composeJacobians(const LhsT_& lhs, const RhsT_& rhs) {
  return makeJacobian<ValuesT_, typename RhsT_::From, typename LhsT_::To>(innerProduct(lhs.getQuantities(), rhs.getQuantities()));
}

}  // namespace internal

/**
 * The Jacobian J = d(To)/d(From) of a change of coordinates from the coordinate system From to the coordinate system To, possibly
 * of another dimension, at one point, e.g. of the task space coordinates of a robot with respect to its joint coordinates: J_ij =
 * dx_i / dq_j for the coordinates q of From and x of To, with the units [x_i] / [q_j]. It is the linearization of the change of
 * coordinates, so it maps the tangent objects at that point, not the points themselves (Lee, Introduction to Smooth Manifolds,
 * 2nd ed., Springer, 2013, chapters 3, 11 and 12, and https://en.wikipedia.org/wiki/Pullback_(differential_geometry)):
 *
 * - transform() pushes forward relative vectors, v' = J v, e.g. velocities, and tensors with two upper indices, T' = J T J^T,
 *   e.g. covariances, from From to To. Points, covectors and tensors with lower indices are rejected.
 * - pullback() pulls covectors, w' = w J, e.g. forces, and tensors with two lower indices, M' = J^T M J, e.g. mass matrices, back
 *   from To to From. It needs no inverse, so it also works for a Jacobian that is not square, e.g. for the joint torques
 *   tau = J^T F that balance a force F at the end effector, or for the joint space inertia J^T M J of a mass matrix M in
 *   Cartesian coordinates. A force is a covector (see ForceCovector): a force vector, e.g. a ForceVector, is pulled back as its
 *   transpose, jacobian.pullback(force.transpose()), which the Euclidean metric of To allows (see Metric).
 *
 * The operational space inertia of Khatib, Lambda = (J M^-1 J^T)^-1 for the joint space inertia M and a Jacobian J of full row
 * rank ("A unified approach for motion and force control of robot manipulators: The operational space formulation", IEEE
 * Journal on Robotics and Automation 3(1), 1987, https://doi.org/10.1109/JRA.1987.1087068; Featherstone and Orin, "Dynamics", in
 * Siciliano and Khatib (eds.), Springer Handbook of Robotics, Springer, 2008, section 2.5.4, "Operational-Space Inertia Matrix"),
 * is the inverse of the pushforward of the inverse of M, which has two upper indices:
 * jacobian.transform(massMatrix.inverse()).inverse().
 * Jacobians compose by the chain rule, J_AC = J_AB * J_BC, in the order of the monogram notation of Drake (see
 * PassiveTransformation), also with changes of basis, i.e. passive linear transformations, rotations and Lorentz
 * transformations, which are the Jacobians of the identity map between two coordinate systems: e.g. R_EW * J_W expresses a task
 * space Jacobian J_W in E instead of W. Only a square Jacobian has an inverse, the Jacobian of the inverse change of coordinates. The
 * matrix maps one coordinate system into another, so it is not a tensor in one coordinate system: getQuantities() returns it,
 * and there is no getTensor().
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_, AnyCoordinateSystem FromT_, AnyCoordinateSystem ToT_,
          int DimensionFromV_, int DimensionToV_, typename... UnitsT_>
  requires AnyValueMatrix<ValuesT_>
class Jacobian
    : public PassiveTransformation<Jacobian<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>, FromT_, ToT_> {
 public:
  using ValueT = ValueT_;
  using ValuesT = ValuesT_<ValueT_, DimensionToV_, DimensionFromV_>;
  using UnitsT = TypeMatrix<DimensionToV_, DimensionFromV_, UnitsT_...>;
  using QuantitiesT = Quantities<ValuesT, UnitsT>;

  // Copies and moves are left to the implicit constructors, see CopiesOrMoves.
  template <typename... Args>
    requires(!CopiesOrMoves<Jacobian, Args...>)
  explicit Jacobian(Args&&... args) : quantities_{std::forward<Args>(args)...} {}

  //! The matrix J, whose element J_ij is the derivative of the coordinate x_i of To with respect to the coordinate q_j of From.
  const QuantitiesT& getQuantities() const { return quantities_; }

  /**
   * The pullback of a covector w in To, w J, e.g. of a force into joint torques, or of a tensor M in To with two lower indices,
   * J^T M J, e.g. of a mass matrix into the joint space inertia, into From (see internal::pullBack()). Other tensors are rejected:
   * transform() pushes forward vectors and tensors with two upper indices.
   */
  template <typename ObjectT_>
  auto pullback(const ObjectT_& object) const {
    static_assert(std::is_same_v<typename GetCoordinateSystem<ObjectT_>::type, ToT_>, "Coordinate systems do not match.");
    return internal::pullBack<FromT_, ToT_>(quantities_, object);
  }

  /**
   * The inverse Jacobian, from To to From, the Jacobian of the inverse change of coordinates, whose matrix is the matrix inverse
   * J^-1 (see Quantities::inverse()). Only a square Jacobian has one, and its units describe a linear map. This needs a value
   * matrix backend with matrix inverses (InvertibleValueMatrixBackend).
   */
  auto inverse() const {
    static_assert(DimensionFromV_ == DimensionToV_,
                  "Only a square Jacobian, between coordinate systems of the same dimension, has an inverse.");
    return internal::makeJacobian<ValuesT_, ToT_, FromT_>(quantities_.inverse());
  }

  //! Whether both Jacobians, which must transform between the same coordinate systems, are approximately equal: their matrices
  //! are compared like the components of tensors (see Tensor::isApprox()), also those that are not square.
  template <template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_, typename FromOtherT_, typename ToOtherT_,
            int DimensionFromOtherV_, int DimensionToOtherV_, typename... UnitsOtherT_>
  bool isApprox(
      const Jacobian<ValuesOtherT_, ValueOtherT_, FromOtherT_, ToOtherT_, DimensionFromOtherV_, DimensionToOtherV_, UnitsOtherT_...>& other,
      const ValueT precision = Precision<ValueT>::value()) const {
    static_assert(internal::SameFrames<Jacobian, std::remove_cvref_t<decltype(other)>>::value);
    return quantities_.isApprox(other.getQuantities(), precision);
  }

 private:
  //! The matrix J, which is multiplied with the quantities of the tensors it maps (see internal::withQuantities()).
  QuantitiesT quantities_;
};

// The tensors that transform() takes are expressed in From (see PassiveTransformation::transform()). A Jacobian pushes forward
// relative vectors, v' = J v, and tensors with two upper indices, T' = J T J^T, like a passive linear transformation (see
// internal::pushForward()), and rejects every other tensor.

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<Jacobian<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  static auto transform(const Jacobian<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>& jacobian,
                        const Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    return internal::pushForward<FromT_, ToT_>(jacobian.getQuantities(), object);
  }
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<Jacobian<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Up, Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>> {
  static auto transform(const Jacobian<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>& jacobian,
                        const Tensor<Indices<Up, Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, Relative>& object) {
    return internal::pushForward<FromT_, ToT_>(jacobian.getQuantities(), object);
  }
};

// A point is mapped by the change of coordinates itself, which is not linear in general: e.g. the Jacobian of the Cartesian
// coordinates with respect to the polar coordinates at (2 m, pi/2) would map that point to (-3.14, 2) m instead of (0, 2) m.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_>
struct TransformationTraits<Jacobian<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<Indices<Up>, QuantitiesOtherT_, CoordinateSystemOtherT_, Absolute>> {
  static_assert(FailOnInstantiation<QuantitiesOtherT_>::value,
                "A Jacobian maps vectors, not points: the change of coordinates whose derivative it is maps the points.");
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_, typename IndicesOtherT_, typename QuantitiesOtherT_, typename CoordinateSystemOtherT_,
          typename AbsoluteOtherT_>
struct TransformationTraits<Jacobian<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>,
                            Tensor<IndicesOtherT_, QuantitiesOtherT_, CoordinateSystemOtherT_, AbsoluteOtherT_>> {
  static_assert(FailOnInstantiation<IndicesOtherT_>::value,
                "A Jacobian pushes forward vectors and tensors with two upper indices. Use pullback() for covectors and tensors with two "
                "lower indices.");
};

// The composition J_AB * J_BC = J_AC of two Jacobians is the chain rule, the product of their matrices.
template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_,
          int DimensionFromLhsV_, int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_,
          typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
struct CompositionTraits<Jacobian<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionFromLhsV_, DimensionToLhsV_, UnitsLhsT_...>,
                         Jacobian<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionFromRhsV_, DimensionToRhsV_, UnitsRhsT_...>> {
  static auto compose(
      const Jacobian<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionFromLhsV_, DimensionToLhsV_, UnitsLhsT_...>& lhs,
      const Jacobian<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionFromRhsV_, DimensionToRhsV_, UnitsRhsT_...>& rhs) {
    return internal::composeJacobians<ValuesLhsT_>(lhs, rhs);
  }
};

// A change of basis, i.e. a passive linear transformation, a rotation or a Lorentz transformation, is the Jacobian of the identity
// map between two coordinate systems, so its compositions with Jacobians, in both orders, are Jacobians, by the chain rule: e.g.
// R_EW * J_W expresses the task space Jacobian J_W in E instead of W.
template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_,
          int DimensionFromLhsV_, int DimensionToLhsV_, typename... UnitsLhsT_, typename RhsT_>
  requires internal::IsChangeOfBasis<RhsT_>::value
struct CompositionTraits<Jacobian<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionFromLhsV_, DimensionToLhsV_, UnitsLhsT_...>,
                         RhsT_> {
  static auto compose(const Jacobian<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionFromLhsV_, DimensionToLhsV_, UnitsLhsT_...>& lhs,
                      const RhsT_& rhs) {
    return internal::composeJacobians<ValuesLhsT_>(lhs, rhs);
  }
};

template <typename LhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_,
          int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
  requires internal::IsChangeOfBasis<LhsT_>::value
struct CompositionTraits<LhsT_,
                         Jacobian<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionFromRhsV_, DimensionToRhsV_, UnitsRhsT_...>> {
  static auto compose(
      const LhsT_& lhs,
      const Jacobian<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionFromRhsV_, DimensionToRhsV_, UnitsRhsT_...>& rhs) {
    return internal::composeJacobians<ValuesRhsT_>(lhs, rhs);
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_,
          int DimensionFromLhsV_, int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_,
          typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
bool operator==(const Jacobian<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionFromLhsV_, DimensionToLhsV_, UnitsLhsT_...>& lhs,
                const Jacobian<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionFromRhsV_, DimensionToRhsV_, UnitsRhsT_...>& rhs) {
  static_assert(internal::SameFrames<std::remove_cvref_t<decltype(lhs)>, std::remove_cvref_t<decltype(rhs)>>::value);
  return lhs.getQuantities() == rhs.getQuantities();
}

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename FromLhsT_, typename ToLhsT_,
          int DimensionFromLhsV_, int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_,
          typename ValueRhsT_, typename FromRhsT_, typename ToRhsT_, int DimensionFromRhsV_, int DimensionToRhsV_, typename... UnitsRhsT_>
bool operator!=(const Jacobian<ValuesLhsT_, ValueLhsT_, FromLhsT_, ToLhsT_, DimensionFromLhsV_, DimensionToLhsV_, UnitsLhsT_...>& lhs,
                const Jacobian<ValuesRhsT_, ValueRhsT_, FromRhsT_, ToRhsT_, DimensionFromRhsV_, DimensionToRhsV_, UnitsRhsT_...>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FromT_, typename ToT_, int DimensionFromV_,
          int DimensionToV_, typename... UnitsT_>
std::ostream& operator<<(std::ostream& lhs,
                         const Jacobian<ValuesT_, ValueT_, FromT_, ToT_, DimensionFromV_, DimensionToV_, UnitsT_...>& rhs) {
  lhs << rhs.getQuantities();
  return lhs;
}

}  // namespace wuzp
