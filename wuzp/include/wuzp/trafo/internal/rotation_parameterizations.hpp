#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/Indices.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Quantity.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/internal/skew_symmetric.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/internal/validity.hpp"
#include "wuzp/trafo/traits.hpp"

namespace wuzp {
namespace internal {

// The parameterizations, the manifold operations and the time derivatives of three-dimensional rotations, shared by
// ActiveSpatialRotation and PassiveSpatialRotation (see "Parameterizations of rotations" in DESIGN.md). They are computed on the
// elements, with the element access of every value matrix backend, and describe the matrix M of a rotation, of an active and of
// a passive one alike, in the convention of robotics: e.g. the quaternion q_AB of the passive rotation from B to A describes its
// matrix R_AB = M(q_AB). Quaternions follow the convention of Hamilton, ij = k, with the real part first.

//! The elements of a three-dimensional vector, e.g. a rotation vector or an angular velocity.
template <typename ValueT_>
using Elements3 = std::array<ValueT_, 3>;

//! The elements (w, x, y, z) of a quaternion w + x i + y j + z k, the real part first.
template <typename ValueT_>
using QuaternionElements = std::array<ValueT_, 4>;

//! Rejects the parameterizations, the manifold operations and the derivatives of rotations that are not three-dimensional, or
//! whose values are not floating point numbers: `static_assert(RequireParameterizable<DimensionV_, ValueT_>::value)`. The value is
//! always true, so that they fail with these messages only.
template <int DimensionV_, typename ValueT_>
struct RequireParameterizable {
  static_assert(DimensionV_ == 3,
                "Quaternions, angle-axis, rotation vectors, Euler angles, boxPlus, boxMinus and the derivatives of rotations are only "
                "defined for three-dimensional rotations.");
  static_assert(std::is_floating_point_v<ValueT_>,
                "The parameterizations, the manifold operations and the derivatives of rotations need floating-point values.");
  static constexpr bool value = true;
};

/**
 * The types of the parameterizations, the manifold operations and the time derivatives of three-dimensional rotations with the
 * value matrix backend ValuesT_ and the value type ValueT_, which ActiveSpatialRotation and PassiveSpatialRotation name. Euler
 * angles and quaternions are not vectors in a coordinate system, so they are quantities, without one; the vectors are tensors in
 * the coordinate system of the rotation, e.g. its axis or its angular velocity.
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_>
struct RotationParameterizationTypes {
  using UnitsT = AccessUnits<AccessImplementation<ValueT_>>;
  using QuaternionT = Quantities<ValuesT_<ValueT_, 4, 1>, TypeMatrix<4, 1, typename UnitsT::Dimensionless>>;
  using QuaternionDerivativeT = Quantities<ValuesT_<ValueT_, 4, 1>, TypeMatrix<4, 1, typename UnitsT::AngularVelocity>>;
  using AngleT = Quantity<ValuesT_<ValueT_, 1, 1>, typename UnitsT::Angle>;
  using EulerAnglesT = Quantities<ValuesT_<ValueT_, 3, 1>, TypeMatrix<3, 1, typename UnitsT::Angle>>;
  using EulerAnglesDerivativeT = Quantities<ValuesT_<ValueT_, 3, 1>, TypeMatrix<3, 1, typename UnitsT::AngularVelocity>>;
  using MatrixDerivativeT = Quantities<ValuesT_<ValueT_, 3, 3>, TypeMatrix<3, 3, typename UnitsT::AngularVelocity>>;
  template <typename CoordinateSystemT_>
  using AxisT = DirectionVector3<ValuesT_, ValueT_, CoordinateSystemT_>;
  template <typename CoordinateSystemT_>
  using RotationVectorT = RotationVector3<ValuesT_, ValueT_, CoordinateSystemT_>;
  template <typename CoordinateSystemT_>
  using AngularVelocityT = AngularVelocityVector3<ValuesT_, ValueT_, CoordinateSystemT_>;
  //! A dimensionless linear map in a coordinate system, e.g. the Jacobian of the exponential map.
  template <typename CoordinateSystemT_>
  using LinearMapT = Tensor<Indices<Up, Down>, Quantities<ValuesT_<ValueT_, 3, 3>, TypeMatrix<3, 3, typename UnitsT::Dimensionless>>,
                            CoordinateSystemT_, Relative>;
};

//! An object constructed from the elements of an array, e.g. a vector of three elements or a matrix of nine, row by row.
template <typename ObjectT_, typename ValueT_, std::size_t SizeV_>
ObjectT_ fromElements(const std::array<ValueT_, SizeV_>& elements) {
  return std::apply([](const auto... element) { return ObjectT_{element...}; }, elements);
}

//! The elements of a column of three values, quantities or a tensor, e.g. of a vector.
template <typename ObjectT_>
auto elements3(const ObjectT_& object) {
  const auto& values = object.getValues();
  return Elements3<typename ObjectT_::ValueT>{values.template get<0, 0>(), values.template get<1, 0>(), values.template get<2, 0>()};
}

//! The elements of a column of four values or quantities, e.g. of a quaternion.
template <typename ObjectT_>
auto quaternionElements(const ObjectT_& object) {
  const auto& values = object.getValues();
  return QuaternionElements<typename ObjectT_::ValueT>{values.template get<0, 0>(), values.template get<1, 0>(),
                                                       values.template get<2, 0>(), values.template get<3, 0>()};
}

template <typename ValueT_>
ValueT_ dot(const Elements3<ValueT_>& lhs, const Elements3<ValueT_>& rhs) {
  return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}

template <typename ValueT_>
Elements3<ValueT_> cross(const Elements3<ValueT_>& lhs, const Elements3<ValueT_>& rhs) {
  return {lhs[1] * rhs[2] - lhs[2] * rhs[1], lhs[2] * rhs[0] - lhs[0] * rhs[2], lhs[0] * rhs[1] - lhs[1] * rhs[0]};
}

template <typename ValueT_>
Elements3<ValueT_> scale(const Elements3<ValueT_>& vector, const ValueT_ factor) {
  return {vector[0] * factor, vector[1] * factor, vector[2] * factor};
}

//! The vector divided by a divisor, e.g. by its norm to normalize it. Unlike a product with the reciprocal, it does not overflow
//! for a subnormal divisor, e.g. the norm of a rotation vector of 1e-310.
template <typename ValueT_>
Elements3<ValueT_> divide(const Elements3<ValueT_>& vector, const ValueT_ divisor) {
  return {vector[0] / divisor, vector[1] / divisor, vector[2] / divisor};
}

template <typename ValueT_>
ValueT_ norm(const Elements3<ValueT_>& vector) {
  return std::hypot(vector[0], vector[1], vector[2]);
}

//! The product M v of a 3x3 matrix and a vector.
template <typename ValuesT_>
auto multiply(const ValuesT_& matrix, const Elements3<typename ValuesT_::ValueT>& vector) {
  return Elements3<typename ValuesT_::ValueT>{
      matrix.template get<0, 0>() * vector[0] + matrix.template get<0, 1>() * vector[1] + matrix.template get<0, 2>() * vector[2],
      matrix.template get<1, 0>() * vector[0] + matrix.template get<1, 1>() * vector[1] + matrix.template get<1, 2>() * vector[2],
      matrix.template get<2, 0>() * vector[0] + matrix.template get<2, 1>() * vector[1] + matrix.template get<2, 2>() * vector[2]};
}

//! The product M^T v of the transpose of a 3x3 matrix and a vector.
template <typename ValuesT_>
auto multiplyTransposed(const ValuesT_& matrix, const Elements3<typename ValuesT_::ValueT>& vector) {
  return Elements3<typename ValuesT_::ValueT>{
      matrix.template get<0, 0>() * vector[0] + matrix.template get<1, 0>() * vector[1] + matrix.template get<2, 0>() * vector[2],
      matrix.template get<0, 1>() * vector[0] + matrix.template get<1, 1>() * vector[1] + matrix.template get<2, 1>() * vector[2],
      matrix.template get<0, 2>() * vector[0] + matrix.template get<1, 2>() * vector[1] + matrix.template get<2, 2>() * vector[2]};
}

//! The Hamilton product p q of two quaternions.
template <typename ValueT_>
QuaternionElements<ValueT_> hamiltonProduct(const QuaternionElements<ValueT_>& lhs, const QuaternionElements<ValueT_>& rhs) {
  return {lhs[0] * rhs[0] - lhs[1] * rhs[1] - lhs[2] * rhs[2] - lhs[3] * rhs[3],
          lhs[0] * rhs[1] + lhs[1] * rhs[0] + lhs[2] * rhs[3] - lhs[3] * rhs[2],
          lhs[0] * rhs[2] - lhs[1] * rhs[3] + lhs[2] * rhs[0] + lhs[3] * rhs[1],
          lhs[0] * rhs[3] + lhs[1] * rhs[2] - lhs[2] * rhs[1] + lhs[3] * rhs[0]};
}

//! The conjugate w - x i - y j - z k of a quaternion, the inverse of a unit quaternion.
template <typename ValueT_>
QuaternionElements<ValueT_> conjugate(const QuaternionElements<ValueT_>& quaternion) {
  return {quaternion[0], -quaternion[1], -quaternion[2], -quaternion[3]};
}

//! The pure quaternion (0, v) of a vector.
template <typename ValueT_>
QuaternionElements<ValueT_> pureQuaternion(const Elements3<ValueT_>& vector) {
  return {ValueT_{0}, vector[0], vector[1], vector[2]};
}

//! The vector part (x, y, z) of a quaternion.
template <typename ValueT_>
Elements3<ValueT_> vectorPart(const QuaternionElements<ValueT_>& quaternion) {
  return {quaternion[1], quaternion[2], quaternion[3]};
}

/**
 * The unique one of the two unit quaternions q and -q of a rotation: the one whose first element that is not zero, in the order
 * w, x, y, z, is positive, like kindr's RotationQuaternion::getUnique(). Its real part w = cos(angle / 2) is not negative, so its
 * angle is in [0, pi].
 */
template <typename ValueT_>
QuaternionElements<ValueT_> uniqueQuaternion(const QuaternionElements<ValueT_>& quaternion) {
  // The first element that is not zero, or zero if all are.
  ValueT_ first{0};
  for (const ValueT_ element : quaternion) {
    if (first == ValueT_{0}) {
      first = element;
    }
  }
  return first < ValueT_{0} ? QuaternionElements<ValueT_>{-quaternion[0], -quaternion[1], -quaternion[2], -quaternion[3]} : quaternion;
}

/**
 * The quaternion normalized to unit length, after checking that it has unit length up to the tolerance of the validity checks:
 * throws std::invalid_argument with the message in debug builds (see checksValidity) if |1 - |q|^2| > ValidityTolerance, e.g.
 * because the axis it was computed from does not have unit length. The matrix of a quaternion that does not have unit length
 * is not a rotation in general, but e.g. that of every quaternion (w, 0, 0, 0) is the identity, which the validity check of
 * the matrix does not reject. The normalization makes the matrix of an accepted quaternion a rotation up to rounding: without
 * it, the deviation of M M^T from the identity would be up to about 4 |1 - |q|^2|, more than the validity check of the matrix
 * accepts. NaN elements fail the check too.
 */
template <typename ValueT_>
QuaternionElements<ValueT_> checkedUnitQuaternion(const QuaternionElements<ValueT_>& quaternion, const char* message) {
  const ValueT_ squaredNorm =
      quaternion[0] * quaternion[0] + quaternion[1] * quaternion[1] + quaternion[2] * quaternion[2] + quaternion[3] * quaternion[3];
  if constexpr (checksValidity) {
    if (!(std::abs(squaredNorm - ValueT_{1}) <= ValidityTolerance<ValueT_>::value())) {
      throw std::invalid_argument(message);
    }
  }
  const ValueT_ norm = std::sqrt(squaredNorm);
  return {quaternion[0] / norm, quaternion[1] / norm, quaternion[2] / norm, quaternion[3] / norm};
}

/**
 * The elements of the direction of a vector, the vector divided by its norm, converted into DirectionT_, e.g. a
 * DirectionVector3 in a coordinate system, which checks that the vector is a vector in it, whose components have the same unit.
 * It first divides the vector by its largest absolute element, so that its norm neither overflows nor underflows, e.g. for
 * float vectors of 2e19 or 1e-25. A zero vector gives NaN.
 */
template <typename DirectionT_, typename VectorT_>
auto directionElements(const VectorT_& vector) {
  const auto elements = elements3(vector);
  const auto scaled = vector / std::max({std::abs(elements[0]), std::abs(elements[1]), std::abs(elements[2])});
  return elements3(DirectionT_{scaled / scaled.norm()});
}

/**
 * The rotation matrix of a unit quaternion q = (w, x, y, z), which rotates a vector v into q v q^*, the vector part of the Hamilton
 * product with the pure quaternion of v (Solà, "Quaternion kinematics for the error-state Kalman filter",
 * https://arxiv.org/abs/1711.02508; Eigen's Quaternion::toRotationMatrix()). A quaternion that does not have unit length gives a
 * matrix that is not a rotation.
 */
template <typename ValuesT_>
ValuesT_ matrixFromQuaternion(const QuaternionElements<typename ValuesT_::ValueT>& quaternion) {
  using ValueT = typename ValuesT_::ValueT;
  const auto [w, x, y, z] = quaternion;
  const ValueT one{1};
  const ValueT two{2};
  return ValuesT_{one - two * (y * y + z * z), two * (x * y - w * z),       two * (x * z + w * y),
                  two * (x * y + w * z),       one - two * (x * x + z * z), two * (y * z - w * x),
                  two * (x * z - w * y),       two * (y * z + w * x),       one - two * (x * x + y * y)};
}

/**
 * The unique unit quaternion (see uniqueQuaternion()) of a rotation matrix, by the method of Shepperd ("Quaternion from rotation
 * matrix", Journal of Guidance and Control 1(3), 1978), which divides by the largest of the
 * four candidates 4 w^2, 4 x^2, 4 y^2 and 4 z^2, so that it is accurate for every rotation, also for angles close to pi.
 */
template <typename ValuesT_>
auto quaternionFromMatrix(const ValuesT_& matrix) {
  using ValueT = typename ValuesT_::ValueT;
  const ValueT m00 = matrix.template get<0, 0>();
  const ValueT m01 = matrix.template get<0, 1>();
  const ValueT m02 = matrix.template get<0, 2>();
  const ValueT m10 = matrix.template get<1, 0>();
  const ValueT m11 = matrix.template get<1, 1>();
  const ValueT m12 = matrix.template get<1, 2>();
  const ValueT m20 = matrix.template get<2, 0>();
  const ValueT m21 = matrix.template get<2, 1>();
  const ValueT m22 = matrix.template get<2, 2>();
  const ValueT one{1};
  const ValueT trace = m00 + m11 + m22;
  QuaternionElements<ValueT> quaternion{};
  if (trace >= m00 && trace >= m11 && trace >= m22) {
    const ValueT s = ValueT{2} * std::sqrt(one + trace);  // 4 w
    quaternion = {s / ValueT{4}, (m21 - m12) / s, (m02 - m20) / s, (m10 - m01) / s};
  } else if (m00 >= m11 && m00 >= m22) {
    const ValueT s = ValueT{2} * std::sqrt(one + m00 - m11 - m22);  // 4 x
    quaternion = {(m21 - m12) / s, s / ValueT{4}, (m01 + m10) / s, (m02 + m20) / s};
  } else if (m11 >= m22) {
    const ValueT s = ValueT{2} * std::sqrt(one + m11 - m00 - m22);  // 4 y
    quaternion = {(m02 - m20) / s, (m01 + m10) / s, s / ValueT{4}, (m12 + m21) / s};
  } else {
    const ValueT s = ValueT{2} * std::sqrt(one + m22 - m00 - m11);  // 4 z
    quaternion = {(m10 - m01) / s, (m02 + m20) / s, (m12 + m21) / s, s / ValueT{4}};
  }
  return uniqueQuaternion(quaternion);
}

//! The unit quaternion (cos(angle / 2), sin(angle / 2) n) of the rotation by an angle around a unit axis n, right-handed.
template <typename ValueT_>
QuaternionElements<ValueT_> quaternionFromAngleAxis(const ValueT_ angle, const Elements3<ValueT_>& axis) {
  const ValueT_ sine = std::sin(angle / ValueT_{2});
  return {std::cos(angle / ValueT_{2}), sine * axis[0], sine * axis[1], sine * axis[2]};
}

/**
 * The angle in [0, pi] and the unit axis of the rotation of a unit quaternion: angle = 2 atan2(|v|, w) and n = v / |v| for its
 * unique form (w, v) (see uniqueQuaternion()). atan2 is accurate for all angles, unlike acos(w) close to 0 and asin(|v|) close to
 * pi. The axis of the identity, whose angle is 0, is arbitrary: it is the x axis, like that of Eigen's AngleAxis.
 */
template <typename ValueT_>
std::pair<ValueT_, Elements3<ValueT_>> angleAxisFromQuaternion(const QuaternionElements<ValueT_>& quaternion) {
  const QuaternionElements<ValueT_> unique = uniqueQuaternion(quaternion);
  const Elements3<ValueT_> vector = vectorPart(unique);
  const ValueT_ vectorNorm = norm(vector);
  if (!(vectorNorm > ValueT_{0})) {
    return {ValueT_{0}, Elements3<ValueT_>{ValueT_{1}, ValueT_{0}, ValueT_{0}}};
  }
  return {ValueT_{2} * std::atan2(vectorNorm, unique[0]), divide(vector, vectorNorm)};
}

//! The unit quaternion of a rotation vector, the rotation by the angle |v| around the axis v / |v|. The rotation vector 0 is the
//! identity.
template <typename ValueT_>
QuaternionElements<ValueT_> quaternionFromRotationVector(const Elements3<ValueT_>& rotationVector) {
  const ValueT_ angle = norm(rotationVector);
  if (!(angle > ValueT_{0})) {
    return {ValueT_{1}, ValueT_{0}, ValueT_{0}, ValueT_{0}};
  }
  return quaternionFromAngleAxis(angle, divide(rotationVector, angle));
}

//! The rotation vector angle * n of a unit quaternion, with the angle in [0, pi] (see angleAxisFromQuaternion()).
template <typename ValueT_>
Elements3<ValueT_> rotationVectorFromQuaternion(const QuaternionElements<ValueT_>& quaternion) {
  const auto [angle, axis] = angleAxisFromQuaternion(quaternion);
  return scale(axis, angle);
}

//! The rotation matrix Exp(v) of a rotation vector v, the exponential map of SO(3).
template <typename ValuesT_>
ValuesT_ matrixFromRotationVector(const Elements3<typename ValuesT_::ValueT>& rotationVector) {
  return matrixFromQuaternion<ValuesT_>(quaternionFromRotationVector(rotationVector));
}

//! The rotation vector Log(M) of a rotation matrix, the logarithmic map of SO(3), with the angle in [0, pi].
template <typename ValuesT_>
auto rotationVectorFromMatrix(const ValuesT_& matrix) {
  return rotationVectorFromQuaternion(quaternionFromMatrix(matrix));
}

//! The left plus M (+) v = Exp(v) M of a rotation matrix M and a rotation vector v, which rotates M further by v (Solà, Deray and
//! Atchuthan, "A micro Lie theory for state estimation in robotics", https://arxiv.org/abs/1812.01537, eq. 27).
template <typename ValuesT_>
ValuesT_ boxPlus(const ValuesT_& matrix, const Elements3<typename ValuesT_::ValueT>& rotationVector) {
  return ValuesT_{matrixFromRotationVector<ValuesT_>(rotationVector).innerProduct(matrix)};
}

//! The left minus M (-) N = Log(M N^T) of two rotation matrices, the rotation vector v with N (+) v = M, with the angle |v| in
//! [0, pi] (Solà et al., eq. 28).
template <typename ValuesT_>
auto boxMinus(const ValuesT_& lhs, const ValuesT_& rhs) {
  return rotationVectorFromMatrix(ValuesT_{lhs.innerProduct(rhs.transpose())});
}

//! The unit quaternion of the rotation by an angle around the coordinate axis AxisV_, 0 for x, 1 for y and 2 for z.
template <int AxisV_, typename ValueT_>
QuaternionElements<ValueT_> quaternionAroundAxis(const ValueT_ angle) {
  QuaternionElements<ValueT_> quaternion{std::cos(angle / ValueT_{2}), ValueT_{0}, ValueT_{0}, ValueT_{0}};
  std::get<AxisV_ + 1>(quaternion) = std::sin(angle / ValueT_{2});
  return quaternion;
}

/**
 * The unit quaternion of the Euler angles (a0, a1, a2) around the axes First, Second and Third, intrinsic: the rotation by a0
 * around the axis First, then by a1 around the new axis Second, then by a2 around the newest axis Third, i.e. M = R_First(a0)
 * R_Second(a1) R_Third(a2), which equals the extrinsic rotations in the opposite order, around the fixed axes Third, Second and
 * First. E.g. the Euler angles ZYX (yaw, pitch, roll) have M = R_z(yaw) R_y(pitch) R_x(roll), like kindr's EulerAnglesZyx and
 * Eigen's eulerAngles(2, 1, 0).
 */
template <int FirstV_, int SecondV_, int ThirdV_, typename ValueT_>
QuaternionElements<ValueT_> quaternionFromEulerAngles(const Elements3<ValueT_>& angles) {
  return hamiltonProduct(hamiltonProduct(quaternionAroundAxis<FirstV_>(angles[0]), quaternionAroundAxis<SecondV_>(angles[1])),
                         quaternionAroundAxis<ThirdV_>(angles[2]));
}

//! The rotation matrix of the Euler angles around the axes First, Second and Third (see quaternionFromEulerAngles()).
template <int FirstV_, int SecondV_, int ThirdV_, typename ValuesT_>
ValuesT_ matrixFromEulerAngles(const Elements3<typename ValuesT_::ValueT>& angles) {
  return matrixFromQuaternion<ValuesT_>(quaternionFromEulerAngles<FirstV_, SecondV_, ThirdV_>(angles));
}

//! An angle of atan2(), in [-pi, pi], in the half-open interval [-pi, pi) of kindr's getUnique(): pi becomes -pi, the same angle.
template <typename ValueT_>
ValueT_ halfOpenAngle(const ValueT_ angle) {
  // NaN stays NaN.
  return angle >= std::numbers::pi_v<ValueT_> ? -std::numbers::pi_v<ValueT_> : angle;
}

/**
 * The Euler angles ZYX (yaw, pitch, roll) of a rotation matrix M = R_z(yaw) R_y(pitch) R_x(roll) (see
 * quaternionFromEulerAngles()), with yaw and roll in [-pi, pi) and pitch in [-pi/2, pi/2], the ranges of kindr's getUnique()
 * (see halfOpenAngle()). The yaw is atan2(M10, M00), and the pitch and the roll follow from R_z(yaw)^T M = R_y(pitch) R_x(roll),
 * like in Eigen's MatrixBase::eulerAngles(), so that they stay accurate close to the gimbal lock at a pitch of +-pi/2, where only
 * the difference or the sum of yaw and roll is defined: there, the yaw is arbitrary, 0 for an exact matrix, and the roll makes
 * up for it.
 */
template <typename ValuesT_>
auto eulerAnglesZyxFromMatrix(const ValuesT_& matrix) {
  using ValueT = typename ValuesT_::ValueT;
  const ValueT yaw = halfOpenAngle(std::atan2(matrix.template get<1, 0>(), matrix.template get<0, 0>()));
  const ValueT cosine = std::cos(yaw);
  const ValueT sine = std::sin(yaw);
  const ValueT pitch = std::atan2(-matrix.template get<2, 0>(), cosine * matrix.template get<0, 0>() + sine * matrix.template get<1, 0>());
  const ValueT roll = std::atan2(sine * matrix.template get<0, 2>() - cosine * matrix.template get<1, 2>(),
                                 cosine * matrix.template get<1, 1>() - sine * matrix.template get<0, 1>());
  return Elements3<ValueT>{yaw, pitch, halfOpenAngle(roll)};
}

/**
 * The Euler angles XYZ (a0, a1, a2) of a rotation matrix M = R_x(a0) R_y(a1) R_z(a2) (see quaternionFromEulerAngles()), with
 * a0 and a2 in [-pi, pi) and a1 in [-pi/2, pi/2], the ranges of kindr's getUnique(). a0 is atan2(-M12, M22), and a1 and a2
 * follow from R_x(a0)^T M = R_y(a1) R_z(a2), so that they stay accurate close to the gimbal lock at a1 = +-pi/2, where a0 is
 * arbitrary, 0 for an exact matrix, and a2 makes up for it (see eulerAnglesZyxFromMatrix()).
 */
template <typename ValuesT_>
auto eulerAnglesXyzFromMatrix(const ValuesT_& matrix) {
  using ValueT = typename ValuesT_::ValueT;
  const ValueT first = halfOpenAngle(std::atan2(-matrix.template get<1, 2>(), matrix.template get<2, 2>()));
  const ValueT cosine = std::cos(first);
  const ValueT sine = std::sin(first);
  const ValueT second = std::atan2(matrix.template get<0, 2>(), cosine * matrix.template get<2, 2>() - sine * matrix.template get<1, 2>());
  const ValueT third = std::atan2(cosine * matrix.template get<1, 0>() + sine * matrix.template get<2, 0>(),
                                  cosine * matrix.template get<1, 1>() + sine * matrix.template get<2, 1>());
  return Elements3<ValueT>{first, second, halfOpenAngle(third)};
}

/**
 * The unit quaternion of the rotation with the smallest angle that rotates the unit vector `from` into the unit vector `to`: by
 * the angle between them around the axis from x to, the normalized quaternion (1 + c, from x to) for c = from . to, halfway
 * between the identity and the rotation by twice the angle, also for nearly opposite vectors (like Eigen's
 * Quaternion::setFromTwoVectors() and kindr's setFromVectors()). Only if the norm of (1 + c, from x to) is below the square root
 * of the machine epsilon, i.e. the vectors are opposite up to that precision, its direction is lost in rounding errors: then
 * every axis perpendicular to them is a solution, up to that precision, and it rotates by pi around the cross product of `from`
 * with the coordinate axis along which `from` has its smallest component, which is far from parallel to it. Both are accurate
 * to about the square root of the machine epsilon at the threshold. NaN elements, e.g. of the direction of a zero vector, give
 * NaN.
 */
template <typename ValueT_>
QuaternionElements<ValueT_> quaternionFromTwoVectors(const Elements3<ValueT_>& from, const Elements3<ValueT_>& to) {
  const ValueT_ one{1};
  const Elements3<ValueT_> crossProduct = cross(from, to);
  const ValueT_ real = one + dot(from, to);
  const ValueT_ halfwayNorm = std::sqrt(real * real + dot(crossProduct, crossProduct));
  // Also NaN takes this branch.
  if (!(halfwayNorm <= std::sqrt(std::numeric_limits<ValueT_>::epsilon()))) {
    const Elements3<ValueT_> axis = divide(crossProduct, halfwayNorm);
    return {real / halfwayNorm, axis[0], axis[1], axis[2]};
  }
  Elements3<ValueT_> coordinateAxis{ValueT_{0}, ValueT_{0}, ValueT_{0}};
  if (std::abs(from[0]) <= std::abs(from[1]) && std::abs(from[0]) <= std::abs(from[2])) {
    coordinateAxis[0] = one;
  } else if (std::abs(from[1]) <= std::abs(from[2])) {
    coordinateAxis[1] = one;
  } else {
    coordinateAxis[2] = one;
  }
  const Elements3<ValueT_> axis = cross(from, coordinateAxis);
  return pureQuaternion(divide(axis, norm(axis)));
}

/**
 * The Jacobian of the exponential map at the rotation vector v, Exp(v + dv) = Exp(J dv) Exp(v) to first order in dv, which also
 * maps the derivative of the rotation vector to the angular velocity w = J dv/dt of dM/dt = [w]x M, for M = Exp(v):
 * J = I + (1 - cos|v|) / |v|^2 [v]x + (|v| - sin|v|) / |v|^3 [v]x^2, the left Jacobian of SO(3) (Solà, Deray and Atchuthan, "A
 * micro Lie theory for state estimation in robotics", https://arxiv.org/abs/1812.01537, eq. 145; kindr's
 * getJacobianOfExponentialMap()). It is computed with the unit axis n = v / |v|, J = I + (2 sin^2(|v| / 2) / |v|) [n]x +
 * (1 - sin|v| / |v|) [n]x^2, which has no cancellation for small angles.
 */
template <typename ValuesT_>
ValuesT_ exponentialMapJacobian(const Elements3<typename ValuesT_::ValueT>& rotationVector) {
  using ValueT = typename ValuesT_::ValueT;
  const ValueT one{1};
  const ValueT angle = norm(rotationVector);
  if (!(angle > ValueT{0})) {
    return identity<ValuesT_>();
  }
  const Elements3<ValueT> axis = divide(rotationVector, angle);
  const ValueT halfSine = std::sin(angle / ValueT{2});
  const ValueT first = ValueT{2} * halfSine * halfSine / angle;
  const ValueT second = one - std::sin(angle) / angle;
  // [n]x^2 = n n^T - I for a unit vector n.
  const auto element = [&](const std::size_t row, const std::size_t col) {
    const ValueT identity = row == col ? one : ValueT{0};
    return identity + second * (axis.at(row) * axis.at(col) - identity);
  };
  return ValuesT_{element(0, 0), element(0, 1) - first * axis[2], element(0, 2) + first * axis[1], element(1, 0) + first * axis[2],
                  element(1, 1), element(1, 2) - first * axis[0], element(2, 0) - first * axis[1], element(2, 1) + first * axis[0],
                  element(2, 2)};
}

//! The derivative dM/dt = [w]x M of a rotation matrix M for its angular velocity w.
template <typename ValuesT_>
ValuesT_ matrixDerivative(const ValuesT_& matrix, const Elements3<typename ValuesT_::ValueT>& angularVelocity) {
  const auto [x, y, z] = angularVelocity;
  return ValuesT_{skewSymmetric<ValuesT_>(x, y, z).innerProduct(matrix)};
}

//! The angular velocity w of the derivative dM/dt of a rotation matrix M, [w]x = dM/dt M^T, the inverse of matrixDerivative(),
//! from the skew-symmetric part of dM/dt M^T, which averages out rounding errors (see vectorOfSkewSymmetricPart()).
template <typename ValuesT_>
auto angularVelocityFromMatrixDerivative(const ValuesT_& matrix, const ValuesT_& derivative) {
  const auto [x, y, z] = vectorOfSkewSymmetricPart(ValuesT_{derivative.innerProduct(matrix.transpose())});
  return Elements3<typename ValuesT_::ValueT>{x, y, z};
}

//! The derivative dq/dt = (0, w) q / 2 of a unit quaternion q of the matrix M, for its angular velocity w with dM/dt = [w]x M
//! (Solà, "Quaternion kinematics for the error-state Kalman filter", https://arxiv.org/abs/1711.02508).
template <typename ValueT_>
QuaternionElements<ValueT_> quaternionDerivative(const QuaternionElements<ValueT_>& quaternion, const Elements3<ValueT_>& angularVelocity) {
  const QuaternionElements<ValueT_> product = hamiltonProduct(pureQuaternion(angularVelocity), quaternion);
  const ValueT_ half = ValueT_{1} / ValueT_{2};
  return {half * product[0], half * product[1], half * product[2], half * product[3]};
}

//! The angular velocity w of dM/dt = [w]x M, the vector part of 2 (dq/dt) q^* for the unit quaternion q of the matrix M, the
//! inverse of quaternionDerivative(). The real part 2 q . dq/dt is zero for the derivative of a unit quaternion.
template <typename ValueT_>
Elements3<ValueT_> angularVelocityFromQuaternionDerivative(const QuaternionElements<ValueT_>& quaternion,
                                                           const QuaternionElements<ValueT_>& derivative) {
  return scale(vectorPart(hamiltonProduct(derivative, conjugate(quaternion))), ValueT_{2});
}

/**
 * The angular velocity w of dM/dt = [w]x M for the Euler angles ZYX (yaw, pitch, roll) of the matrix M = R_z(yaw) R_y(pitch)
 * R_x(roll) and their derivatives: w = dyaw/dt e_z + dpitch/dt R_z(yaw) e_y + droll/dt R_z(yaw) R_y(pitch) e_x, the sum of the
 * angular velocities around the axes of the three rotations, each expressed after the rotations before it.
 */
template <typename ValueT_>
Elements3<ValueT_> angularVelocityFromEulerAnglesZyxDerivative(const Elements3<ValueT_>& angles, const Elements3<ValueT_>& derivative) {
  const ValueT_ cosYaw = std::cos(angles[0]);
  const ValueT_ sinYaw = std::sin(angles[0]);
  const ValueT_ cosPitch = std::cos(angles[1]);
  const ValueT_ sinPitch = std::sin(angles[1]);
  return {-sinYaw * derivative[1] + cosYaw * cosPitch * derivative[2], cosYaw * derivative[1] + sinYaw * cosPitch * derivative[2],
          derivative[0] - sinPitch * derivative[2]};
}

//! The derivatives of the Euler angles ZYX for the angular velocity w, the inverse of
//! angularVelocityFromEulerAnglesZyxDerivative(). At the gimbal lock, cos(pitch) = 0, they are not defined: the results are not finite or
//! arbitrarily large.
template <typename ValueT_>
Elements3<ValueT_> eulerAnglesZyxDerivative(const Elements3<ValueT_>& angles, const Elements3<ValueT_>& angularVelocity) {
  const ValueT_ cosYaw = std::cos(angles[0]);
  const ValueT_ sinYaw = std::sin(angles[0]);
  const ValueT_ rollDerivative = (cosYaw * angularVelocity[0] + sinYaw * angularVelocity[1]) / std::cos(angles[1]);
  return {angularVelocity[2] + std::sin(angles[1]) * rollDerivative, -sinYaw * angularVelocity[0] + cosYaw * angularVelocity[1],
          rollDerivative};
}

//! The angular velocity w of dM/dt = [w]x M for the Euler angles XYZ (a0, a1, a2) of the matrix M = R_x(a0) R_y(a1) R_z(a2) and
//! their derivatives: w = da0/dt e_x + da1/dt R_x(a0) e_y + da2/dt R_x(a0) R_y(a1) e_z.
template <typename ValueT_>
Elements3<ValueT_> angularVelocityFromEulerAnglesXyzDerivative(const Elements3<ValueT_>& angles, const Elements3<ValueT_>& derivative) {
  const ValueT_ cosFirst = std::cos(angles[0]);
  const ValueT_ sinFirst = std::sin(angles[0]);
  const ValueT_ cosSecond = std::cos(angles[1]);
  return {derivative[0] + std::sin(angles[1]) * derivative[2], cosFirst * derivative[1] - sinFirst * cosSecond * derivative[2],
          sinFirst * derivative[1] + cosFirst * cosSecond * derivative[2]};
}

//! The derivatives of the Euler angles XYZ for the angular velocity w, the inverse of
//! angularVelocityFromEulerAnglesXyzDerivative(). At the gimbal lock, cos(a1) = 0, they are not defined: the results are not finite or
//! arbitrarily large.
template <typename ValueT_>
Elements3<ValueT_> eulerAnglesXyzDerivative(const Elements3<ValueT_>& angles, const Elements3<ValueT_>& angularVelocity) {
  const ValueT_ cosFirst = std::cos(angles[0]);
  const ValueT_ sinFirst = std::sin(angles[0]);
  const ValueT_ thirdDerivative = (-sinFirst * angularVelocity[1] + cosFirst * angularVelocity[2]) / std::cos(angles[1]);
  return {angularVelocity[0] - std::sin(angles[1]) * thirdDerivative, cosFirst * angularVelocity[1] + sinFirst * angularVelocity[2],
          thirdDerivative};
}

/**
 * Converts the components of a vector of the tangent space of a passive rotation from FromT_ to ToT_, e.g. an angular velocity or
 * a rotation vector of boxPlus(), between From and To, given the matrix R of the rotation: the components in To are R times those
 * in From, like those of every vector. InFromV_ is whether the components are, or are to be, expressed in From. These are
 * specializations rather than `if constexpr`: MSVC compiles both branches of an `if constexpr` in member functions of class
 * templates (see mapRelative).
 */
template <bool InFromV_>
struct TangentComponents {
  template <typename ValuesT_>
  static Elements3<typename ValuesT_::ValueT> toTo(const ValuesT_& /*matrix*/, const Elements3<typename ValuesT_::ValueT>& vector) {
    return vector;
  }
  template <typename ValuesT_>
  static Elements3<typename ValuesT_::ValueT> fromTo(const ValuesT_& /*matrix*/, const Elements3<typename ValuesT_::ValueT>& vector) {
    return vector;
  }
};
template <>
struct TangentComponents<true> {
  template <typename ValuesT_>
  static Elements3<typename ValuesT_::ValueT> toTo(const ValuesT_& matrix, const Elements3<typename ValuesT_::ValueT>& vector) {
    return multiply(matrix, vector);
  }
  template <typename ValuesT_>
  static Elements3<typename ValuesT_::ValueT> fromTo(const ValuesT_& matrix, const Elements3<typename ValuesT_::ValueT>& vector) {
    return multiplyTransposed(matrix, vector);
  }
};

//! Whether a vector of the tangent space of a passive rotation from FromT_ to ToT_ is expressed in From rather than in To, e.g.
//! the rotation vector of boxPlus() or an angular velocity, which must be expressed in one of them (see TangentComponents).
template <typename FromT_, typename ToT_, typename CoordinateSystemT_>
struct IsInFrom {
  static_assert(std::is_same_v<CoordinateSystemT_, FromT_> || std::is_same_v<CoordinateSystemT_, ToT_>,
                "The vector must be expressed in From or To of the passive rotation.");
  static constexpr bool value = !std::is_same_v<CoordinateSystemT_, ToT_>;
};

}  // namespace internal
}  // namespace wuzp
