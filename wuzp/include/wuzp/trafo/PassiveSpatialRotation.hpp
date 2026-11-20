#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformationTemplate.hpp"
#include "wuzp/trafo/PassiveTransformation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"
#include "wuzp/trafo/internal/rigid.hpp"
#include "wuzp/trafo/internal/rotation.hpp"
#include "wuzp/trafo/internal/rotation_parameterizations.hpp"
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
 * In three dimensions, a rotation converts from and to unit quaternions, angle-axis, rotation vectors and Euler angles ZYX and
 * XYZ, has the manifold operations boxPlus(), boxMinus() and angularDistance(), and maps between its angular velocity and the
 * time derivatives of its matrix, its quaternion and its Euler angles, like ActiveSpatialRotation. Its parameters describe its
 * matrix R in the convention of robotics, like those of the active rotation with the same matrix: e.g. the quaternion q_AB of the
 * passive rotation from B to A gives R_AB = R(q_AB), the orientation of B relative to A, like Drake's RotationMatrix R_AB
 * (https://drake.mit.edu/doxygen_cxx/group__multibody__notation.html) and kindr's C_AB. Its angular velocity w is that of From
 * relative to To, dR/dt = [w]x R for w expressed in To and R [w]x for w expressed in From, which replaces kindr's global (To) and
 * local (From) angular velocities (see "Parameterizations of rotations" in DESIGN.md and docs/conventions.md). These need
 * floating-point values.
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_, EuclideanCoordinateSystem FromT_,
          EuclideanCoordinateSystem ToT_, int DimensionV_>
  requires AnyValueMatrix<ValuesT_>
class PassiveSpatialRotation
    : public PassiveTransformation<PassiveSpatialRotation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_>, FromT_, ToT_> {
  using ParameterizationsT = internal::RotationParameterizationTypes<ValuesT_, ValueT_>;
  using RequireParameterizableT = internal::RequireParameterizable<DimensionV_, ValueT_>;
  using AngleUnitT = typename ParameterizationsT::UnitsT::Angle;
  using AngularVelocityUnitT = typename ParameterizationsT::UnitsT::AngularVelocity;

 public:
  //! The passive linear transformation with the matrix of the rotation, whose transformations the rotation shares.
  using LinearTransformationT = PassiveLinearTransformation<ValuesT_, ValueT_, FromT_, ToT_, DimensionV_,
                                                            typename AccessUnits<AccessImplementation<ValueT_>>::Dimensionless>;
  using ValueT = ValueT_;
  using ValuesT = typename LinearTransformationT::ValuesT;
  using UnitsT = typename LinearTransformationT::UnitsT;
  using QuantitiesT = typename LinearTransformationT::QuantitiesT;

  //! A unit quaternion (w, x, y, z) of the matrix (see ActiveSpatialRotation::QuaternionT), e.g. q_AB of the rotation from B to A.
  using QuaternionT = typename ParameterizationsT::QuaternionT;
  //! The derivative dq/dt of a quaternion with respect to time, in the unit of an angular velocity.
  using QuaternionDerivativeT = typename ParameterizationsT::QuaternionDerivativeT;
  //! An angle, e.g. of a rotation around its axis, or between two rotations.
  using AngleT = typename ParameterizationsT::AngleT;
  //! The unit axis of a rotation, expressed in To. The rotation leaves its axis unchanged, so its components in From are the same.
  using AxisT = typename ParameterizationsT::template AxisT<ToT_>;
  //! The angle and the unit axis of a rotation.
  using AngleAxisT = std::pair<AngleT, AxisT>;
  //! A rotation vector angle * axis, expressed in To, e.g. the logarithm of a rotation or an increment of boxPlus().
  using RotationVectorT = typename ParameterizationsT::template RotationVectorT<ToT_>;
  //! Three Euler angles, in the order of the axes in their name, e.g. (yaw, pitch, roll) for ZYX. They are not a vector, so they
  //! have no coordinate system.
  using EulerAnglesT = typename ParameterizationsT::EulerAnglesT;
  //! The derivatives of three Euler angles with respect to time.
  using EulerAnglesDerivativeT = typename ParameterizationsT::EulerAnglesDerivativeT;
  //! The angular velocity of From relative to To, expressed in To. It may also be expressed in From.
  using AngularVelocityT = typename ParameterizationsT::template AngularVelocityT<ToT_>;
  //! The derivative dR/dt of the matrix of a rotation with respect to time.
  using MatrixDerivativeT = typename ParameterizationsT::MatrixDerivativeT;

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

  // The parameterizations of three-dimensional rotations (see "Parameterizations of rotations" in DESIGN.md), which describe the
  // matrix R of the rotation, like those of the ActiveSpatialRotation with the same matrix. A vector of the tangent space, e.g. an
  // angular velocity, may be expressed in To or in From; results are expressed in the coordinate system of the template argument,
  // To by default.

  //! The rotation of the unit quaternion q, whose matrix is that of ActiveSpatialRotation::fromQuaternion(): e.g. the quaternion
  //! q_AB of the passive rotation from B to A gives R_AB, like Drake's RotationMatrix(q_AB) and kindr's C_AB.
  //! @throws std::invalid_argument In debug builds, if the quaternion does not have unit length (see
  //! ActiveSpatialRotation::fromQuaternion()).
  template <AnyQuantities QuaternionOtherT_>
  static PassiveSpatialRotation fromQuaternion(const QuaternionOtherT_& quaternion) {
    static_assert(RequireParameterizableT::value);
    const auto elements = internal::checkedUnitQuaternion(internal::quaternionElements(QuaternionT{quaternion}),
                                                          "The quaternion of a rotation must have unit length.");
    return makeUnchecked(internal::matrixFromQuaternion<ValuesT>(elements));
  }

  //! The unique unit quaternion of the matrix (see ActiveSpatialRotation::toQuaternion()).
  QuaternionT toQuaternion() const {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<QuaternionT>(internal::quaternionFromMatrix(quantities_.getValues()));
  }

  //! The rotation whose matrix rotates by an angle around a unit axis, right-handed, which is expressed in To or in From, where
  //! its components are the same.
  //! @throws std::invalid_argument In debug builds, if the axis does not have unit length (see
  //! ActiveSpatialRotation::fromAngleAxis()).
  template <AnyQuantities AngleOtherT_, AnyTensor AxisOtherT_>
  static PassiveSpatialRotation fromAngleAxis(const AngleOtherT_& angle, const AxisOtherT_& axis) {
    static_assert(RequireParameterizableT::value);
    const auto elements = internal::checkedUnitQuaternion(
        internal::quaternionFromAngleAxis(AngleT{angle}.getValue(), components<typename ParameterizationsT::UnitsT::Dimensionless>(axis)),
        "The axis of a rotation must have unit length.");
    return makeUnchecked(internal::matrixFromQuaternion<ValuesT>(elements));
  }

  //! The angle in [0, pi] and the unit axis of the matrix (see ActiveSpatialRotation::toAngleAxis()).
  AngleAxisT toAngleAxis() const {
    static_assert(RequireParameterizableT::value);
    const auto [angle, axis] = internal::angleAxisFromQuaternion(internal::quaternionFromMatrix(quantities_.getValues()));
    return {AngleT{angle}, internal::fromElements<AxisT>(axis)};
  }

  //! The rotation whose matrix is the exponential map Exp(v) of the rotation vector v, which is expressed in To or in From, where
  //! its components are the same, since it is along the axis of the rotation.
  template <AnyTensor RotationVectorOtherT_>
  static PassiveSpatialRotation fromRotationVector(const RotationVectorOtherT_& rotationVector) {
    static_assert(RequireParameterizableT::value);
    return makeUnchecked(internal::matrixFromRotationVector<ValuesT>(components<AngleUnitT>(rotationVector)));
  }

  //! The logarithmic map Log(R) of the matrix, the rotation vector of the rotation, with an angle in [0, pi].
  RotationVectorT toRotationVector() const {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<RotationVectorT>(internal::rotationVectorFromMatrix(quantities_.getValues()));
  }

  //! The rotation whose matrix has the Euler angles ZYX (yaw, pitch, roll), R = R_z(yaw) R_y(pitch) R_x(roll) (see
  //! ActiveSpatialRotation::fromEulerAnglesZyx()), like kindr's EulerAnglesZyx.
  template <AnyQuantities EulerAnglesOtherT_>
  static PassiveSpatialRotation fromEulerAnglesZyx(const EulerAnglesOtherT_& angles) {
    static_assert(RequireParameterizableT::value);
    return makeUnchecked(internal::matrixFromEulerAngles<2, 1, 0, ValuesT>(internal::elements3(EulerAnglesT{angles})));
  }

  //! The Euler angles ZYX of the matrix (see ActiveSpatialRotation::toEulerAnglesZyx()).
  EulerAnglesT toEulerAnglesZyx() const {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<EulerAnglesT>(internal::eulerAnglesZyxFromMatrix(quantities_.getValues()));
  }

  //! The rotation whose matrix has the Euler angles XYZ (a0, a1, a2), R = R_x(a0) R_y(a1) R_z(a2) (see
  //! ActiveSpatialRotation::fromEulerAnglesXyz()), like kindr's EulerAnglesXyz.
  template <AnyQuantities EulerAnglesOtherT_>
  static PassiveSpatialRotation fromEulerAnglesXyz(const EulerAnglesOtherT_& angles) {
    static_assert(RequireParameterizableT::value);
    return makeUnchecked(internal::matrixFromEulerAngles<0, 1, 2, ValuesT>(internal::elements3(EulerAnglesT{angles})));
  }

  //! The Euler angles XYZ of the matrix (see ActiveSpatialRotation::toEulerAnglesXyz()).
  EulerAnglesT toEulerAnglesXyz() const {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<EulerAnglesT>(internal::eulerAnglesXyzFromMatrix(quantities_.getValues()));
  }

  /**
   * The rotation with the smallest angle under which the direction of the vector `from`, expressed in From, has the direction of
   * the vector `to`, expressed in To: transform(from) has the direction of `to`, e.g. the gravity measured by an accelerometer in
   * From and the gravity in To. Its matrix is that of ActiveSpatialRotation::fromTwoVectors(), like kindr's
   * C_TF.setFromVectors(from, to).
   * @throws std::invalid_argument In debug builds, if a vector is zero, so that it has no direction (see isValid()).
   */
  template <AnyTensor FromVectorT_, AnyTensor ToVectorT_>
  static PassiveSpatialRotation fromTwoVectors(const FromVectorT_& from, const ToVectorT_& to) {
    static_assert(RequireParameterizableT::value);
    return PassiveSpatialRotation{internal::matrixFromQuaternion<ValuesT>(internal::quaternionFromTwoVectors(
        internal::directionElements<typename ParameterizationsT::template AxisT<FromT_>>(from), internal::directionElements<AxisT>(to)))};
  }

  // The rotations as a manifold (see ActiveSpatialRotation::boxPlus()), with the perturbations of the matrix on the left in To,
  // like kindr's boxPlus() and boxMinus() of C_TF, or equivalently on the right in From.

  //! The rotation whose matrix R is rotated further by the rotation vector v: Exp(v) R for v in To, which equals R Exp(v) for v in
  //! From, so that the same rotation vector gives the same result in both, like kindr's boxPlus() of C_TF with v in To.
  template <AnyTensor RotationVectorOtherT_>
  PassiveSpatialRotation boxPlus(const RotationVectorOtherT_& rotationVector) const {
    static_assert(RequireParameterizableT::value);
    const ValuesT& matrix = quantities_.getValues();
    return makeUnchecked(internal::boxPlus(matrix, componentsInTo<AngleUnitT>(matrix, rotationVector)));
  }

  //! The rotation vector Log(R S^T) of the matrices R of this rotation and S of the other one, expressed in To or, with the template
  //! argument From, in From: other.boxPlus(boxMinus(other)) is this rotation, like kindr's boxMinus() of C_TF in To.
  template <typename CoordinateSystemT_ = ToT_, typename FromOtherT_, typename ToOtherT_>
  auto boxMinus(const PassiveSpatialRotation<ValuesT_, ValueT_, FromOtherT_, ToOtherT_, DimensionV_>& other) const {
    static_assert(internal::SameFrames<PassiveSpatialRotation, std::remove_cvref_t<decltype(other)>>::value);
    static_assert(RequireParameterizableT::value);
    const ValuesT& matrix = quantities_.getValues();
    return vectorFromTo<CoordinateSystemT_, AngleUnitT>(matrix, internal::boxMinus(matrix, other.getQuantities().getValues()));
  }

  //! The angle in [0, pi] of the rotation between this and the other rotation, the norm of boxMinus(other), like kindr's
  //! getDisparityAngle().
  template <typename FromOtherT_, typename ToOtherT_>
  AngleT angularDistance(const PassiveSpatialRotation<ValuesT_, ValueT_, FromOtherT_, ToOtherT_, DimensionV_>& other) const {
    static_assert(internal::SameFrames<PassiveSpatialRotation, std::remove_cvref_t<decltype(other)>>::value);
    static_assert(RequireParameterizableT::value);
    return AngleT{internal::norm(internal::boxMinus(quantities_.getValues(), other.getQuantities().getValues()))};
  }

  /**
   * The Jacobian J of the exponential map at the rotation vector v, a linear map in the coordinate system of v, To or From, in
   * which it maps the derivative of v to the angular velocity, w = J dv/dt, and in which J dv is the increment of boxPlus(),
   * Exp(v + dv) = Exp(v).boxPlus(J dv) to first order. In To, it is the left Jacobian J_l(v) (see
   * ActiveSpatialRotation::exponentialMapJacobian()), and in From the right Jacobian J_r(v) = J_l(-v) = J_l(v)^T = R^T J_l(v)
   * (Solà, Deray and Atchuthan, "A micro Lie theory for state estimation in robotics", https://arxiv.org/abs/1812.01537, eqs. 143
   * and 145). The components of v are the same in both.
   */
  template <AnyTensor RotationVectorOtherT_>
  static auto exponentialMapJacobian(const RotationVectorOtherT_& rotationVector) {
    static_assert(RequireParameterizableT::value);
    using CoordinateSystemOtherT = typename GetCoordinateSystem<RotationVectorOtherT_>::type;
    constexpr bool inFrom = internal::IsInFrom<FromT_, ToT_, CoordinateSystemOtherT>::value;
    // The right Jacobian in From is the transpose of the left one, by a specialization rather than a conditional operator on
    // inFrom, which is known at compile time, so that it leaves no code that coverage reports as never executed.
    return typename ParameterizationsT::template LinearMapT<CoordinateSystemOtherT>{
        internal::TransposeIf<inFrom>::value(internal::exponentialMapJacobian<ValuesT>(components<AngleUnitT>(rotationVector)))};
  }

  // The time derivatives of rotations and the angular velocity w of From relative to To, dR/dt = [w]x R for w in To, which equals
  // R [w]x for w in From, like kindr's GlobalAngularVelocity (To) and LocalAngularVelocity (From) of C_TF.

  //! The derivative dR/dt = [w]x R of the matrix for the angular velocity w in To, which equals R [w]x for w in From, like kindr's
  //! RotationMatrixDiff.
  template <AnyTensor AngularVelocityOtherT_>
  MatrixDerivativeT matrixDerivative(const AngularVelocityOtherT_& angularVelocity) const {
    static_assert(RequireParameterizableT::value);
    const ValuesT& matrix = quantities_.getValues();
    return MatrixDerivativeT{internal::matrixDerivative(matrix, componentsInTo<AngularVelocityUnitT>(matrix, angularVelocity))};
  }

  //! The angular velocity of the derivative dR/dt of the matrix, [w]x = dR/dt R^T in To, expressed in To or, with the template
  //! argument From, in From, the inverse of matrixDerivative().
  template <typename CoordinateSystemT_ = ToT_, AnyQuantities MatrixDerivativeOtherT_>
  auto angularVelocityFromMatrixDerivative(const MatrixDerivativeOtherT_& derivative) const {
    static_assert(RequireParameterizableT::value);
    const ValuesT& matrix = quantities_.getValues();
    return vectorFromTo<CoordinateSystemT_, AngularVelocityUnitT>(
        matrix, internal::angularVelocityFromMatrixDerivative(matrix, MatrixDerivativeT{derivative}.getValues()));
  }

  //! The derivative dq/dt = (0, w) q / 2 of the unit quaternion q for the angular velocity w in To, which equals q (0, w) / 2 for
  //! w in From, like kindr's RotationQuaternionDiff of a GlobalAngularVelocity or a LocalAngularVelocity.
  template <AnyQuantities QuaternionOtherT_, AnyTensor AngularVelocityOtherT_>
  static QuaternionDerivativeT quaternionDerivative(const QuaternionOtherT_& quaternion, const AngularVelocityOtherT_& angularVelocity) {
    static_assert(RequireParameterizableT::value);
    const auto elements = internal::quaternionElements(QuaternionT{quaternion});
    return internal::fromElements<QuaternionDerivativeT>(internal::quaternionDerivative(
        elements, componentsInTo<AngularVelocityUnitT>(internal::matrixFromQuaternion<ValuesT>(elements), angularVelocity)));
  }

  //! The angular velocity of the derivative dq/dt of the unit quaternion q, expressed in To or, with the template argument From,
  //! in From, the inverse of quaternionDerivative().
  template <typename CoordinateSystemT_ = ToT_, AnyQuantities QuaternionOtherT_, AnyQuantities QuaternionDerivativeOtherT_>
  static auto angularVelocityFromQuaternionDerivative(const QuaternionOtherT_& quaternion, const QuaternionDerivativeOtherT_& derivative) {
    static_assert(RequireParameterizableT::value);
    const auto elements = internal::quaternionElements(QuaternionT{quaternion});
    return vectorFromTo<CoordinateSystemT_, AngularVelocityUnitT>(
        internal::matrixFromQuaternion<ValuesT>(elements),
        internal::angularVelocityFromQuaternionDerivative(elements, internal::quaternionElements(QuaternionDerivativeT{derivative})));
  }

  //! The derivatives of the Euler angles ZYX of the matrix for the angular velocity w, in To or in From, like kindr's
  //! EulerAnglesZyxDiff (see ActiveSpatialRotation::eulerAnglesZyxDerivative()).
  template <AnyQuantities EulerAnglesOtherT_, AnyTensor AngularVelocityOtherT_>
  static EulerAnglesDerivativeT eulerAnglesZyxDerivative(const EulerAnglesOtherT_& angles, const AngularVelocityOtherT_& angularVelocity) {
    static_assert(RequireParameterizableT::value);
    const auto elements = internal::elements3(EulerAnglesT{angles});
    return internal::fromElements<EulerAnglesDerivativeT>(internal::eulerAnglesZyxDerivative(
        elements, componentsInTo<AngularVelocityUnitT>(internal::matrixFromEulerAngles<2, 1, 0, ValuesT>(elements), angularVelocity)));
  }

  //! The angular velocity of the derivatives of the Euler angles ZYX of the matrix, expressed in To or, with the template
  //! argument From, in From, the inverse of eulerAnglesZyxDerivative().
  template <typename CoordinateSystemT_ = ToT_, AnyQuantities EulerAnglesOtherT_, AnyQuantities EulerAnglesDerivativeOtherT_>
  static auto angularVelocityFromEulerAnglesZyxDerivative(const EulerAnglesOtherT_& angles,
                                                          const EulerAnglesDerivativeOtherT_& derivative) {
    static_assert(RequireParameterizableT::value);
    const auto elements = internal::elements3(EulerAnglesT{angles});
    return vectorFromTo<CoordinateSystemT_, AngularVelocityUnitT>(
        internal::matrixFromEulerAngles<2, 1, 0, ValuesT>(elements),
        internal::angularVelocityFromEulerAnglesZyxDerivative(elements, internal::elements3(EulerAnglesDerivativeT{derivative})));
  }

  //! The derivatives of the Euler angles XYZ of the matrix for the angular velocity w, in To or in From, like kindr's
  //! EulerAnglesXyzDiff (see ActiveSpatialRotation::eulerAnglesXyzDerivative()).
  template <AnyQuantities EulerAnglesOtherT_, AnyTensor AngularVelocityOtherT_>
  static EulerAnglesDerivativeT eulerAnglesXyzDerivative(const EulerAnglesOtherT_& angles, const AngularVelocityOtherT_& angularVelocity) {
    static_assert(RequireParameterizableT::value);
    const auto elements = internal::elements3(EulerAnglesT{angles});
    return internal::fromElements<EulerAnglesDerivativeT>(internal::eulerAnglesXyzDerivative(
        elements, componentsInTo<AngularVelocityUnitT>(internal::matrixFromEulerAngles<0, 1, 2, ValuesT>(elements), angularVelocity)));
  }

  //! The angular velocity of the derivatives of the Euler angles XYZ of the matrix, expressed in To or, with the template
  //! argument From, in From, the inverse of eulerAnglesXyzDerivative().
  template <typename CoordinateSystemT_ = ToT_, AnyQuantities EulerAnglesOtherT_, AnyQuantities EulerAnglesDerivativeOtherT_>
  static auto angularVelocityFromEulerAnglesXyzDerivative(const EulerAnglesOtherT_& angles,
                                                          const EulerAnglesDerivativeOtherT_& derivative) {
    static_assert(RequireParameterizableT::value);
    const auto elements = internal::elements3(EulerAnglesT{angles});
    return vectorFromTo<CoordinateSystemT_, AngularVelocityUnitT>(
        internal::matrixFromEulerAngles<0, 1, 2, ValuesT>(elements),
        internal::angularVelocityFromEulerAnglesXyzDerivative(elements, internal::elements3(EulerAnglesDerivativeT{derivative})));
  }

 private:
  //! The components of a vector expressed in To or in From, in its own coordinate system, which is converted into a vector with
  //! the unit UnitT_, e.g. to check its unit. A vector along the axis of the rotation, e.g. its rotation vector, has the same
  //! components in both.
  template <typename UnitT_, typename VectorT_>
  static auto components(const VectorT_& vector) {
    using CoordinateSystemOtherT = typename GetCoordinateSystem<VectorT_>::type;
    // Rejects other coordinate systems than From and To.
    static_cast<void>(internal::IsInFrom<FromT_, ToT_, CoordinateSystemOtherT>::value);
    return internal::elements3(HomogeneousVector<3, ValuesT_, ValueT_, UnitT_, CoordinateSystemOtherT>{vector});
  }

  //! The components in To of a vector of the tangent space, e.g. an angular velocity, expressed in To or in From, which is
  //! converted into a vector with the unit UnitT_, e.g. to check its unit, given the matrix R (see internal::TangentComponents).
  template <typename UnitT_, typename VectorT_>
  static auto componentsInTo(const ValuesT& matrix, const VectorT_& vector) {
    constexpr bool inFrom = internal::IsInFrom<FromT_, ToT_, typename GetCoordinateSystem<VectorT_>::type>::value;
    return internal::TangentComponents<inFrom>::toTo(matrix, components<UnitT_>(vector));
  }

  //! The vector with the unit UnitT_, expressed in CoordinateSystemT_, To or From, whose components in To are given, given the
  //! matrix R (see internal::TangentComponents).
  template <typename CoordinateSystemT_, typename UnitT_>
  static auto vectorFromTo(const ValuesT& matrix, const internal::Elements3<ValueT_>& elementsInTo) {
    constexpr bool inFrom = internal::IsInFrom<FromT_, ToT_, CoordinateSystemT_>::value;
    return internal::fromElements<HomogeneousVector<3, ValuesT_, ValueT_, UnitT_, CoordinateSystemT_>>(
        internal::TangentComponents<inFrom>::fromTo(matrix, elementsInTo));
  }

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

}  // namespace wuzp
