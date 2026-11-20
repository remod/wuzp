#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveLinearTransformation.hpp"
#include "wuzp/trafo/ActiveProperRigidTransformationTemplate.hpp"
#include "wuzp/trafo/ActiveTransformation.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"
#include "wuzp/trafo/internal/rigid.hpp"
#include "wuzp/trafo/internal/rotation.hpp"
#include "wuzp/trafo/internal/rotation_parameterizations.hpp"
#include "wuzp/trafo/internal/validity.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {

/**
 * An active rotation in its coordinate system: the dimensionless linear transformation x -> R x whose matrix R is orthogonal
 * with determinant 1, R R^T = I and det(R) = 1. Orthogonality is defined by the Euclidean metric, so the coordinate system must
 * be Euclidean (EuclideanCoordinateSystem, see Metric); in a Minkowski coordinate system, a spatial rotation is a
 * LorentzTransformation. It transforms the same tensors as the active linear transformation with its matrix (see
 * ActiveLinearTransformation). It composes with rotations into rotations, with linear transformations into linear
 * transformations, and with translations and proper rigid transformations into proper rigid transformations (see
 * CompositionTraits). Its inverse is its transpose, which every value matrix backend provides, also for integer matrices, and
 * with which it transforms covectors and tensors with lower indices.
 * In debug builds, i.e. if NDEBUG is not defined, the constructors check that the matrix is a rotation (see isValid()), like
 * Drake's RotationMatrix (https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rotation_matrix.html): they throw
 * std::invalid_argument if it is not, e.g. for a shear, a scaling or a reflection. Release builds do not check. makeUnchecked()
 * skips the check, e.g. in hot paths, and projectToRotation() computes the rotation nearest to a matrix, e.g. to one whose
 * rounding errors accumulated.
 * In three dimensions, a rotation converts from and to unit quaternions, angle-axis, rotation vectors and Euler angles ZYX and
 * XYZ, has the manifold operations boxPlus(), boxMinus() and angularDistance(), and maps between its angular velocity w,
 * dR/dt = [w]x R, which is expressed in its coordinate system, and the time derivatives of its matrix, its quaternion and its
 * Euler angles (see "Parameterizations of rotations" in DESIGN.md and docs/conventions.md). These need floating-point values.
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_, EuclideanCoordinateSystem CoordinateSystemT_, int DimensionV_>
  requires AnyValueMatrix<ValuesT_>
class ActiveSpatialRotation : public ActiveTransformation<ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>> {
  using ParameterizationsT = internal::RotationParameterizationTypes<ValuesT_, ValueT_>;
  using RequireParameterizableT = internal::RequireParameterizable<DimensionV_, ValueT_>;

 public:
  //! The active linear transformation with the matrix of the rotation, whose transformations the rotation shares.
  using LinearTransformationT = ActiveLinearTransformation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_, DimensionV_,
                                                           typename AccessUnits<AccessImplementation<ValueT_>>::Dimensionless>;
  using ValueT = ValueT_;
  using ValuesT = typename LinearTransformationT::ValuesT;
  using UnitsT = typename LinearTransformationT::UnitsT;
  using QuantitiesT = typename LinearTransformationT::QuantitiesT;
  using CoordinateSystemT = CoordinateSystemT_;
  using TensorT = typename LinearTransformationT::TensorT;

  //! A unit quaternion (w, x, y, z) = (cos(angle / 2), sin(angle / 2) n) of the rotation by an angle around the unit axis n, in
  //! the convention of Hamilton, ij = k, with the real part w first, like Eigen's Quaternion(w, x, y, z) and kindr's
  //! RotationQuaternion(w, x, y, z), but unlike the storage order (x, y, z, w) of Eigen's coeffs(). It rotates a vector v into
  //! q v q^*, and -q is the same rotation.
  using QuaternionT = typename ParameterizationsT::QuaternionT;
  //! The derivative dq/dt of a quaternion with respect to time, in the unit of an angular velocity.
  using QuaternionDerivativeT = typename ParameterizationsT::QuaternionDerivativeT;
  //! An angle, e.g. of a rotation around its axis, or between two rotations.
  using AngleT = typename ParameterizationsT::AngleT;
  //! The unit axis of a rotation, expressed in the coordinate system.
  using AxisT = typename ParameterizationsT::template AxisT<CoordinateSystemT_>;
  //! The angle and the unit axis of a rotation.
  using AngleAxisT = std::pair<AngleT, AxisT>;
  //! A rotation vector angle * axis, expressed in the coordinate system, e.g. the logarithm of a rotation or an increment of
  //! boxPlus().
  using RotationVectorT = typename ParameterizationsT::template RotationVectorT<CoordinateSystemT_>;
  //! Three Euler angles, in the order of the axes in their name, e.g. (yaw, pitch, roll) for ZYX. They are not a vector, so they
  //! have no coordinate system.
  using EulerAnglesT = typename ParameterizationsT::EulerAnglesT;
  //! The derivatives of three Euler angles with respect to time.
  using EulerAnglesDerivativeT = typename ParameterizationsT::EulerAnglesDerivativeT;
  //! The angular velocity w of a rotation R, dR/dt = [w]x R, expressed in the coordinate system.
  using AngularVelocityT = typename ParameterizationsT::template AngularVelocityT<CoordinateSystemT_>;
  //! The derivative dR/dt of the matrix of a rotation with respect to time.
  using MatrixDerivativeT = typename ParameterizationsT::MatrixDerivativeT;

  //! The identity.
  ActiveSpatialRotation() : quantities_{internal::identity<ValuesT>()} {}

  //! The rotation whose matrix is constructed from the arguments, e.g. its elements row by row, or its quantities. Copies and moves
  //! are left to the implicit constructors, see CopiesOrMoves.
  //! @throws std::invalid_argument In debug builds, if the matrix is not a rotation (see isValid()).
  template <typename... Args>
    requires(sizeof...(Args) > 0 && !CopiesOrMoves<ActiveSpatialRotation, Args...>)
  explicit ActiveSpatialRotation(Args&&... args) : quantities_{std::forward<Args>(args)...} {
    internal::checkValidity(*this, "The matrix of a rotation must be orthogonal with determinant 1.");
  }

  //! The rotation whose matrix is constructed from the arguments, without checking it, like Drake's
  //! RotationMatrix::MakeUnchecked(): the caller guarantees that the matrix is a rotation, e.g. a product of rotations.
  template <typename... Args>
  static ActiveSpatialRotation makeUnchecked(Args&&... args) {
    return ActiveSpatialRotation{Unchecked{}, QuantitiesT{std::forward<Args>(args)...}};
  }

  //! The rotation nearest to the matrix constructed from the arguments, e.g. to a rotation whose rounding errors accumulated, by
  //! the Newton-Schulz iteration (see internal::projectToRotation()).
  //! @throws std::invalid_argument In every build, if the matrix A is not close enough to a rotation, i.e. ||I - A^T A|| is not
  //! smaller than 1 in the Frobenius norm, or if its determinant is not positive.
  template <typename... Args>
  static ActiveSpatialRotation projectToRotation(Args&&... args) {
    return makeUnchecked(internal::projectToRotation(QuantitiesT{std::forward<Args>(args)...}.getValues()));
  }

  //! The matrix R, which transforms a vector v into R v.
  const QuantitiesT& getQuantities() const { return quantities_; }
  //! The matrix as a linear map, a tensor with an upper and a lower index.
  TensorT getTensor() const { return TensorT{quantities_}; }

  //! The inverse rotation, whose matrix is the transpose R^T = R^-1: it needs no matrix inverse.
  ActiveSpatialRotation inverse() const { return makeUnchecked(quantities_.transpose()); }

  //! Whether the matrix is a rotation: max_ij |(R R^T - I)_ij| <= tolerance, and det(R) > 0 (see internal::isRotation()). The
  //! constructors check it in debug builds, with the tolerance ValidityTolerance.
  bool isValid(const ValueT tolerance = ValidityTolerance<ValueT>::value()) const {
    return internal::isRotation(quantities_.getValues(), tolerance);
  }

  //! Whether both rotations, which must be expressed in the same coordinate system, are approximately equal: their matrices are
  //! compared like the components of tensors (see Tensor::isApprox()).
  template <template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_, typename CoordinateSystemOtherT_,
            int DimensionOtherV_>
  bool isApprox(const ActiveSpatialRotation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionOtherV_>& other,
                const ValueT precision = Precision<ValueT>::value()) const {
    static_assert(std::is_same_v<CoordinateSystemT_, CoordinateSystemOtherT_>, "Coordinate systems do not match.");
    return quantities_.isApprox(other.getQuantities(), precision);
  }

  // The parameterizations of three-dimensional rotations (see "Parameterizations of rotations" in DESIGN.md). The arguments are
  // converted into the types of the parameterizations, so that e.g. Euler angles with another unit fail with "Units do not
  // match.", and a rotation vector in another coordinate system with "Coordinate systems do not match.".

  //! The rotation of a unit quaternion q (see QuaternionT), whose matrix is that of Eigen's Quaternion::toRotationMatrix().
  //! The quaternion is normalized, after it is checked to have unit length up to the tolerance of the validity checks.
  //! @throws std::invalid_argument In debug builds, if the quaternion does not have unit length, |1 - |q|^2| > ValidityTolerance
  //! (see internal::checkedUnitQuaternion()), like kindr's assertion. Normalize it first, e.g. after integrating its derivative.
  template <AnyQuantities QuaternionOtherT_>
  static ActiveSpatialRotation fromQuaternion(const QuaternionOtherT_& quaternion) {
    static_assert(RequireParameterizableT::value);
    const auto elements = internal::checkedUnitQuaternion(internal::quaternionElements(QuaternionT{quaternion}),
                                                          "The quaternion of a rotation must have unit length.");
    return makeUnchecked(internal::matrixFromQuaternion<ValuesT>(elements));
  }

  //! The unique unit quaternion of the rotation, whose first element that is not zero is positive, like kindr's
  //! RotationQuaternion::getUnique(): its real part w is not negative, so its angle is in [0, pi] (see
  //! internal::quaternionFromMatrix()). Like every choice of one of q and -q, it jumps close to an angle of pi, where w changes
  //! its sign: compare rotations with isApprox() or angularDistance(), not with their quaternions.
  QuaternionT toQuaternion() const {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<QuaternionT>(internal::quaternionFromMatrix(quantities_.getValues()));
  }

  //! The right-handed rotation by an angle around a unit axis, which is expressed in the coordinate system, like Eigen's
  //! AngleAxis.
  //! @throws std::invalid_argument In debug builds, if the axis does not have unit length, unless the angle is a multiple of
  //! 2 pi (see internal::checkedUnitQuaternion()), which normalizes the quaternion of the angle and the axis.
  template <AnyQuantities AngleOtherT_, AnyTensor AxisOtherT_>
  static ActiveSpatialRotation fromAngleAxis(const AngleOtherT_& angle, const AxisOtherT_& axis) {
    static_assert(RequireParameterizableT::value);
    const auto elements =
        internal::checkedUnitQuaternion(internal::quaternionFromAngleAxis(AngleT{angle}.getValue(), internal::elements3(AxisT{axis})),
                                        "The axis of a rotation must have unit length.");
    return makeUnchecked(internal::matrixFromQuaternion<ValuesT>(elements));
  }

  //! The angle in [0, pi] and the unit axis of the rotation. The axis of the identity is arbitrary: the x axis (see
  //! internal::angleAxisFromQuaternion()).
  AngleAxisT toAngleAxis() const {
    static_assert(RequireParameterizableT::value);
    const auto [angle, axis] = internal::angleAxisFromQuaternion(internal::quaternionFromMatrix(quantities_.getValues()));
    return {AngleT{angle}, internal::fromElements<AxisT>(axis)};
  }

  //! The exponential map Exp(v) of a rotation vector v, which is expressed in the coordinate system: the rotation by the angle |v|
  //! around the axis v / |v|, like kindr's RotationVector and exponentialMap().
  template <AnyTensor RotationVectorOtherT_>
  static ActiveSpatialRotation fromRotationVector(const RotationVectorOtherT_& rotationVector) {
    static_assert(RequireParameterizableT::value);
    return makeUnchecked(internal::matrixFromRotationVector<ValuesT>(internal::elements3(RotationVectorT{rotationVector})));
  }

  //! The logarithmic map Log(R), the rotation vector of the rotation, whose norm, the angle, is in [0, pi], like kindr's
  //! logarithmicMap().
  RotationVectorT toRotationVector() const {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<RotationVectorT>(internal::rotationVectorFromMatrix(quantities_.getValues()));
  }

  //! The rotation of the Euler angles ZYX (yaw, pitch, roll), intrinsic: by yaw around z, then by pitch around the new y, then
  //! by roll around the newest x, R = R_z(yaw) R_y(pitch) R_x(roll), like kindr's EulerAnglesZyx and Eigen's
  //! eulerAngles(2, 1, 0). These are the extrinsic rotations by roll around x, by pitch around y and by yaw around z.
  template <AnyQuantities EulerAnglesOtherT_>
  static ActiveSpatialRotation fromEulerAnglesZyx(const EulerAnglesOtherT_& angles) {
    static_assert(RequireParameterizableT::value);
    return makeUnchecked(internal::matrixFromEulerAngles<2, 1, 0, ValuesT>(internal::elements3(EulerAnglesT{angles})));
  }

  //! The Euler angles ZYX (yaw, pitch, roll) of the rotation (see fromEulerAnglesZyx()), with yaw and roll in [-pi, pi) and
  //! pitch in [-pi/2, pi/2]. At the gimbal lock, a pitch of +-pi/2, only the sum or the difference of yaw and roll is defined
  //! (see internal::eulerAnglesZyxFromMatrix()).
  EulerAnglesT toEulerAnglesZyx() const {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<EulerAnglesT>(internal::eulerAnglesZyxFromMatrix(quantities_.getValues()));
  }

  //! The rotation of the Euler angles XYZ (a0, a1, a2), intrinsic: by a0 around x, then by a1 around the new y, then by a2
  //! around the newest z, R = R_x(a0) R_y(a1) R_z(a2), like kindr's EulerAnglesXyz and Eigen's eulerAngles(0, 1, 2).
  template <AnyQuantities EulerAnglesOtherT_>
  static ActiveSpatialRotation fromEulerAnglesXyz(const EulerAnglesOtherT_& angles) {
    static_assert(RequireParameterizableT::value);
    return makeUnchecked(internal::matrixFromEulerAngles<0, 1, 2, ValuesT>(internal::elements3(EulerAnglesT{angles})));
  }

  //! The Euler angles XYZ (a0, a1, a2) of the rotation (see fromEulerAnglesXyz()), with a0 and a2 in [-pi, pi) and a1 in
  //! [-pi/2, pi/2] (see internal::eulerAnglesXyzFromMatrix()).
  EulerAnglesT toEulerAnglesXyz() const {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<EulerAnglesT>(internal::eulerAnglesXyzFromMatrix(quantities_.getValues()));
  }

  /**
   * The rotation with the smallest angle that rotates the direction of the vector `from` into the direction of the vector `to`,
   * both expressed in the coordinate system: transform(from) has the direction of `to`, like Eigen's
   * Quaternion::FromTwoVectors() and kindr's setFromVectors(). Only their directions matter, so their units may differ, but the
   * components of each must have the same unit. If they are opposite, it rotates by pi around an axis perpendicular to them (see
   * internal::quaternionFromTwoVectors()).
   * @throws std::invalid_argument In debug builds, if a vector is zero, so that it has no direction (see isValid()).
   */
  template <AnyTensor FromVectorT_, AnyTensor ToVectorT_>
  static ActiveSpatialRotation fromTwoVectors(const FromVectorT_& from, const ToVectorT_& to) {
    static_assert(RequireParameterizableT::value);
    return ActiveSpatialRotation{internal::matrixFromQuaternion<ValuesT>(
        internal::quaternionFromTwoVectors(internal::directionElements<AxisT>(from), internal::directionElements<AxisT>(to)))};
  }

  // The rotations as a manifold, in the conventions of Solà, Deray and Atchuthan, "A micro Lie theory for state estimation in
  // robotics" (https://arxiv.org/abs/1812.01537), with the perturbations on the left, expressed in the coordinate system, like
  // kindr's boxPlus() and boxMinus().

  //! The rotation R (+) v = Exp(v) R, rotated further by the rotation vector v, which is expressed in the coordinate system: the
  //! left plus of Solà et al. (eq. 27), like kindr's boxPlus().
  template <AnyTensor RotationVectorOtherT_>
  ActiveSpatialRotation boxPlus(const RotationVectorOtherT_& rotationVector) const {
    static_assert(RequireParameterizableT::value);
    return makeUnchecked(internal::boxPlus(quantities_.getValues(), internal::elements3(RotationVectorT{rotationVector})));
  }

  //! The rotation vector R (-) S = Log(R S^T), expressed in the coordinate system, which rotates the other rotation S into this
  //! one: other.boxPlus(boxMinus(other)) is this rotation. The left minus of Solà et al. (eq. 28), like kindr's boxMinus().
  template <typename CoordinateSystemOtherT_>
  RotationVectorT boxMinus(const ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemOtherT_, DimensionV_>& other) const {
    static_assert(std::is_same_v<CoordinateSystemT_, CoordinateSystemOtherT_>, "Coordinate systems do not match.");
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<RotationVectorT>(internal::boxMinus(quantities_.getValues(), other.getQuantities().getValues()));
  }

  //! The angle in [0, pi] of the rotation between this and the other rotation, the norm of boxMinus(other), like kindr's
  //! getDisparityAngle() and Eigen's Quaternion::angularDistance().
  template <typename CoordinateSystemOtherT_>
  AngleT angularDistance(const ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemOtherT_, DimensionV_>& other) const {
    static_assert(std::is_same_v<CoordinateSystemT_, CoordinateSystemOtherT_>, "Coordinate systems do not match.");
    static_assert(RequireParameterizableT::value);
    return AngleT{internal::norm(internal::boxMinus(quantities_.getValues(), other.getQuantities().getValues()))};
  }

  //! The Jacobian J of the exponential map at the rotation vector v, a linear map in the coordinate system: Exp(v + dv) =
  //! Exp(J dv) Exp(v) to first order, so that it maps the derivative of v to the angular velocity, w = J dv/dt, like kindr's
  //! getJacobianOfExponentialMap(). It is the left Jacobian of SO(3) (see internal::exponentialMapJacobian()).
  template <AnyTensor RotationVectorOtherT_>
  static TensorT exponentialMapJacobian(const RotationVectorOtherT_& rotationVector) {
    static_assert(RequireParameterizableT::value);
    return TensorT{internal::exponentialMapJacobian<ValuesT>(internal::elements3(RotationVectorT{rotationVector}))};
  }

  // The time derivatives of rotations and their angular velocity w, dR/dt = [w]x R, which is expressed in the coordinate system,
  // like kindr's GlobalAngularVelocity.

  //! The derivative dR/dt = [w]x R of the matrix for the angular velocity w, like kindr's RotationMatrixDiff.
  template <AnyTensor AngularVelocityOtherT_>
  MatrixDerivativeT matrixDerivative(const AngularVelocityOtherT_& angularVelocity) const {
    static_assert(RequireParameterizableT::value);
    return MatrixDerivativeT{internal::matrixDerivative(quantities_.getValues(), internal::elements3(AngularVelocityT{angularVelocity}))};
  }

  //! The angular velocity w of the derivative dR/dt of the matrix, [w]x = dR/dt R^T, the inverse of matrixDerivative(). It
  //! takes the skew-symmetric part, which averages out rounding errors.
  template <AnyQuantities MatrixDerivativeOtherT_>
  AngularVelocityT angularVelocityFromMatrixDerivative(const MatrixDerivativeOtherT_& derivative) const {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<AngularVelocityT>(
        internal::angularVelocityFromMatrixDerivative(quantities_.getValues(), MatrixDerivativeT{derivative}.getValues()));
  }

  //! The derivative dq/dt = (0, w) q / 2 of the unit quaternion q for the angular velocity w, like kindr's RotationQuaternionDiff
  //! of a GlobalAngularVelocity. It depends on the sign of q, so it takes the quaternion rather than the rotation.
  template <AnyQuantities QuaternionOtherT_, AnyTensor AngularVelocityOtherT_>
  static QuaternionDerivativeT quaternionDerivative(const QuaternionOtherT_& quaternion, const AngularVelocityOtherT_& angularVelocity) {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<QuaternionDerivativeT>(internal::quaternionDerivative(
        internal::quaternionElements(QuaternionT{quaternion}), internal::elements3(AngularVelocityT{angularVelocity})));
  }

  //! The angular velocity w of the derivative dq/dt of the unit quaternion q, the vector part of 2 (dq/dt) q^*, the inverse of
  //! quaternionDerivative().
  template <AnyQuantities QuaternionOtherT_, AnyQuantities QuaternionDerivativeOtherT_>
  static AngularVelocityT angularVelocityFromQuaternionDerivative(const QuaternionOtherT_& quaternion,
                                                                  const QuaternionDerivativeOtherT_& derivative) {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<AngularVelocityT>(internal::angularVelocityFromQuaternionDerivative(
        internal::quaternionElements(QuaternionT{quaternion}), internal::quaternionElements(QuaternionDerivativeT{derivative})));
  }

  //! The derivatives of the Euler angles ZYX (yaw, pitch, roll) for the angular velocity w, like kindr's EulerAnglesZyxDiff.
  //! They depend on the angles, not only on the rotation, so it takes the angles. At the gimbal lock, cos(pitch) = 0, they are
  //! not defined: the results are not finite or arbitrarily large.
  template <AnyQuantities EulerAnglesOtherT_, AnyTensor AngularVelocityOtherT_>
  static EulerAnglesDerivativeT eulerAnglesZyxDerivative(const EulerAnglesOtherT_& angles, const AngularVelocityOtherT_& angularVelocity) {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<EulerAnglesDerivativeT>(internal::eulerAnglesZyxDerivative(
        internal::elements3(EulerAnglesT{angles}), internal::elements3(AngularVelocityT{angularVelocity})));
  }

  //! The angular velocity w of the derivatives of the Euler angles ZYX, w = dyaw/dt e_z + dpitch/dt R_z(yaw) e_y + droll/dt
  //! R_z(yaw) R_y(pitch) e_x, the inverse of eulerAnglesZyxDerivative().
  template <AnyQuantities EulerAnglesOtherT_, AnyQuantities EulerAnglesDerivativeOtherT_>
  static AngularVelocityT angularVelocityFromEulerAnglesZyxDerivative(const EulerAnglesOtherT_& angles,
                                                                      const EulerAnglesDerivativeOtherT_& derivative) {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<AngularVelocityT>(internal::angularVelocityFromEulerAnglesZyxDerivative(
        internal::elements3(EulerAnglesT{angles}), internal::elements3(EulerAnglesDerivativeT{derivative})));
  }

  //! The derivatives of the Euler angles XYZ for the angular velocity w, like kindr's EulerAnglesXyzDiff. At the gimbal lock,
  //! cos(a1) = 0, they are not defined: the results are not finite or arbitrarily large.
  template <AnyQuantities EulerAnglesOtherT_, AnyTensor AngularVelocityOtherT_>
  static EulerAnglesDerivativeT eulerAnglesXyzDerivative(const EulerAnglesOtherT_& angles, const AngularVelocityOtherT_& angularVelocity) {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<EulerAnglesDerivativeT>(internal::eulerAnglesXyzDerivative(
        internal::elements3(EulerAnglesT{angles}), internal::elements3(AngularVelocityT{angularVelocity})));
  }

  //! The angular velocity w of the derivatives of the Euler angles XYZ, w = da0/dt e_x + da1/dt R_x(a0) e_y + da2/dt R_x(a0)
  //! R_y(a1) e_z, the inverse of eulerAnglesXyzDerivative().
  template <AnyQuantities EulerAnglesOtherT_, AnyQuantities EulerAnglesDerivativeOtherT_>
  static AngularVelocityT angularVelocityFromEulerAnglesXyzDerivative(const EulerAnglesOtherT_& angles,
                                                                      const EulerAnglesDerivativeOtherT_& derivative) {
    static_assert(RequireParameterizableT::value);
    return internal::fromElements<AngularVelocityT>(internal::angularVelocityFromEulerAnglesXyzDerivative(
        internal::elements3(EulerAnglesT{angles}), internal::elements3(EulerAnglesDerivativeT{derivative})));
  }

 private:
  struct Unchecked {};
  ActiveSpatialRotation(Unchecked /*tag*/, QuantitiesT quantities) : quantities_{std::move(quantities)} {}

  //! The matrix R, which transforms a vector v into R v.
  QuantitiesT quantities_;
};

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ActiveSpatialRotation2 = ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, 2>;
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_>
using ActiveSpatialRotation3 = ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, 3>;

namespace internal {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionV_>
struct IsRotation<ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>> : std::true_type {};

}  // namespace internal

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionV_>
struct GetCoordinateSystem<ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>> {
  using type = CoordinateSystemT_;
};

// A rotation transforms tensors, and conjugates translations and linear transformations, like the active linear transformation
// with its matrix, but with its transpose as the inverse, e.g. for covectors.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionV_, typename Object>
struct TransformationTraits<ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>, Object>
    : TransformationTraits<typename ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>::LinearTransformationT,
                           Object> {};

// The conjugation R Q R^T of a rotation Q by a rotation R is a rotation.
template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionV_,
          template <typename, int, int> typename ValuesOtherT_, typename ValueOtherT_, typename CoordinateSystemOtherT_,
          int DimensionOtherV_>
struct TransformationTraits<ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>,
                            ActiveSpatialRotation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionOtherV_>> {
  using ObjectT = ActiveSpatialRotation<ValuesOtherT_, ValueOtherT_, CoordinateSystemOtherT_, DimensionOtherV_>;
  static ObjectT transform(const ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>& transformation,
                           const ObjectT& object) {
    static_assert(std::is_same_v<CoordinateSystemT_, CoordinateSystemOtherT_>, "Coordinate systems do not match.");
    const auto& rotation = transformation.getQuantities();
    return ObjectT::makeUnchecked(innerProduct(rotation, innerProduct(object.getQuantities(), rotation.transpose())));
  }
};

// The composition of two rotations is the rotation R_lhs R_rhs.
template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename CoordinateSystemRhsT_, int DimensionRhsV_>
struct CompositionTraits<ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>,
                         ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>> {
  using ResultT = ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>;
  static ResultT compose(const ResultT& lhs,
                         const ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>& rhs) {
    return ResultT::makeUnchecked(innerProduct(lhs.getQuantities(), rhs.getQuantities()));
  }
};

// A rotation is a linear transformation: its compositions with linear transformations are those of the linear transformation with
// its matrix, the products of the matrices.
template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename CoordinateSystemRhsT_, int DimensionFromRhsV_,
          int DimensionToRhsV_, typename... UnitsRhsT_>
struct CompositionTraits<
    ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>,
    ActiveLinearTransformation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionFromRhsV_, DimensionToRhsV_, UnitsRhsT_...>>
    : CompositionTraits<
          typename ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>::LinearTransformationT,
          ActiveLinearTransformation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionFromRhsV_, DimensionToRhsV_, UnitsRhsT_...>> {
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionFromLhsV_,
          int DimensionToLhsV_, typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_,
          typename CoordinateSystemRhsT_, int DimensionRhsV_>
struct CompositionTraits<
    ActiveLinearTransformation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionFromLhsV_, DimensionToLhsV_, UnitsLhsT_...>,
    ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>>
    : CompositionTraits<
          ActiveLinearTransformation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionFromLhsV_, DimensionToLhsV_, UnitsLhsT_...>,
          typename ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>::LinearTransformationT> {};

// The compositions of a rotation with a translation or a proper rigid transformation are proper rigid transformations: of the
// rotation and the translation, or of the type of the proper rigid operand (see internal/rigid.hpp).

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, int DimensionRhsV_, typename ValueRhsT_, typename CoordinateSystemRhsT_,
          typename... UnitsRhsT_>
struct CompositionTraits<ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>,
                         ActiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, UnitsRhsT_...>> {
  using LhsT = ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>;
  using RhsT = ActiveTranslation<ValuesRhsT_, DimensionRhsV_, ValueRhsT_, CoordinateSystemRhsT_, UnitsRhsT_...>;
  using ResultT = ActiveProperRigidTransformationTemplate<CoordinateSystemLhsT_, LhsT, RhsT>;
  static ResultT compose(const LhsT& lhs, const RhsT& rhs) { return internal::composeRotationAndTranslation<ResultT>(lhs, rhs); }
};

template <template <typename, int, int> typename ValuesLhsT_, int DimensionLhsV_, typename ValueLhsT_, typename CoordinateSystemLhsT_,
          typename... UnitsLhsT_, template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename CoordinateSystemRhsT_,
          int DimensionRhsV_>
struct CompositionTraits<ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>,
                         ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>> {
  using LhsT = ActiveTranslation<ValuesLhsT_, DimensionLhsV_, ValueLhsT_, CoordinateSystemLhsT_, UnitsLhsT_...>;
  using RhsT = ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>;
  using ResultT = ActiveProperRigidTransformationTemplate<CoordinateSystemRhsT_, RhsT, LhsT>;
  static ResultT compose(const LhsT& lhs, const RhsT& rhs) { return internal::composeTranslationAndRotation<ResultT>(lhs, rhs); }
};

template <typename CoordinateSystemT_, typename RotationT_, typename TranslationT_, template <typename, int, int> typename ValuesRhsT_,
          typename ValueRhsT_, typename CoordinateSystemRhsT_, int DimensionRhsV_>
struct CompositionTraits<ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>,
                         ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>> {
  using ResultT = ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>;
  static ResultT compose(const ResultT& lhs,
                         const ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>& rhs) {
    return internal::composeRigidAndRotation<ResultT>(lhs, rhs);
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionLhsV_,
          typename CoordinateSystemT_, typename RotationT_, typename TranslationT_>
struct CompositionTraits<ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>,
                         ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>> {
  using ResultT = ActiveProperRigidTransformationTemplate<CoordinateSystemT_, RotationT_, TranslationT_>;
  static ResultT compose(const ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>& lhs,
                         const ResultT& rhs) {
    return internal::composeRotationAndRigid<ResultT>(lhs, rhs);
  }
};

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename CoordinateSystemRhsT_, int DimensionRhsV_>
bool operator==(const ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>& lhs,
                const ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>& rhs) {
  static_assert(std::is_same_v<CoordinateSystemLhsT_, CoordinateSystemRhsT_>, "Coordinate systems do not match.");
  return lhs.getQuantities() == rhs.getQuantities();
}

template <template <typename, int, int> typename ValuesLhsT_, typename ValueLhsT_, typename CoordinateSystemLhsT_, int DimensionLhsV_,
          template <typename, int, int> typename ValuesRhsT_, typename ValueRhsT_, typename CoordinateSystemRhsT_, int DimensionRhsV_>
bool operator!=(const ActiveSpatialRotation<ValuesLhsT_, ValueLhsT_, CoordinateSystemLhsT_, DimensionLhsV_>& lhs,
                const ActiveSpatialRotation<ValuesRhsT_, ValueRhsT_, CoordinateSystemRhsT_, DimensionRhsV_>& rhs) {
  return !(lhs == rhs);
}

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename CoordinateSystemT_, int DimensionV_>
std::ostream& operator<<(std::ostream& lhs, const ActiveSpatialRotation<ValuesT_, ValueT_, CoordinateSystemT_, DimensionV_>& rhs) {
  lhs << rhs.getQuantities();
  return lhs;
}

}  // namespace wuzp
