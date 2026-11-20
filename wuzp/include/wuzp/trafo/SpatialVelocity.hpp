#pragma once

#include <type_traits>
#include <utility>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformationTemplate.hpp"
#include "wuzp/trafo/SpatialVector.hpp"
#include "wuzp/trafo/kinematics.hpp"

namespace wuzp {

/**
 * The spatial velocity (twist) V_MBp_E of the frame B, measured in the frame M, about the point Bp, expressed in the coordinate
 * system E, in the monogram notation of Drake's SpatialVelocity
 * (https://drake.mit.edu/doxygen_cxx/classdrake_1_1multibody_1_1_spatial_velocity.html): the angular velocity w_MB of B relative
 * to M and the velocity v_MBp of the point Bp, which moves with B, relative to M, both expressed in E, the rotational part first,
 * like Drake and Modern Robotics (Lynch and Park, Cambridge University Press, 2017, section 3.3.2). The frames, the point and the
 * coordinate system are coordinate systems of wuzp, which must be Euclidean: a point is the origin of a coordinate system, e.g.
 * `using Tip = CoordinateSystem<TipId>;` for the tip of a tool fixed to B, and the origin Bo of B is B itself. The tags are
 * checked at compile time: spatial velocities are only added and compared if all their tags match, and
 * shift() and composeWithMovingFrameVelocity() change them (see "Spatial vectors" in DESIGN.md).
 * A passive rotation or a passive proper rigid transformation from E to F re-expresses it in F, e.g. `X_FE.transform(velocity)`,
 * which rotates both parts, like vectors, and only changes E: the point stays Bp, unlike that of the adjoint map, adjoint(),
 * which also moves the point to the origin of F. A coordinate system that moves needs composeWithMovingFrameVelocity() instead.
 * Active transformations do not transform it. The angular velocity of PassiveSpatialRotation<From = B, To = A>,
 * that of B relative to A, is the rotational part of a SpatialVelocity<A, B, P, E> with E = A or E = B, e.g. the result of
 * angularVelocityFromMatrixDerivative(). adjoint() maps it by a passive proper rigid transformation, like the adjoint map of
 * Modern Robotics. Its angular velocity is in 1/s, since angles are dimensionless (see "Angles are dimensionless" in DESIGN.md).
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_, EuclideanCoordinateSystem MeasuredInT_,
          EuclideanCoordinateSystem FrameT_, EuclideanCoordinateSystem PointT_, EuclideanCoordinateSystem ExpressedInT_>
  requires AnyValueMatrix<ValuesT_>
class SpatialVelocity
    : public SpatialVector<SpatialVelocity<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointT_, ExpressedInT_>,
                           AngularVelocityVector3<ValuesT_, ValueT_, ExpressedInT_>, VelocityVector3<ValuesT_, ValueT_, ExpressedInT_>> {
  using BaseT = SpatialVector<SpatialVelocity, AngularVelocityVector3<ValuesT_, ValueT_, ExpressedInT_>,
                              VelocityVector3<ValuesT_, ValueT_, ExpressedInT_>>;

 public:
  //! The frame M the motion is measured in.
  using MeasuredInT = MeasuredInT_;
  //! The frame B whose motion it is.
  using FrameT = FrameT_;
  //! The point Bp the spatial velocity is about, which is fixed to B.
  using PointT = PointT_;
  //! The coordinate system E both parts are expressed in.
  using ExpressedInT = ExpressedInT_;
  //! The position vector from one point to another, expressed in E, by which shift() shifts.
  using OffsetT = PositionVector3<ValuesT_, ValueT_, ExpressedInT_>;

  SpatialVelocity() = default;

  //! The spatial velocity with the angular velocity w_MB and the velocity v_MBp, which are converted into the angular velocity and
  //! the velocity vectors in E: e.g. swapped parts fail with "Units do not match.".
  template <typename AngularVelocityOtherT_, typename VelocityOtherT_>
    requires(AnyTensor<std::remove_cvref_t<AngularVelocityOtherT_>> && AnyTensor<std::remove_cvref_t<VelocityOtherT_>>)
  SpatialVelocity(AngularVelocityOtherT_&& angularVelocity, VelocityOtherT_&& velocity)
      : BaseT(std::forward<AngularVelocityOtherT_>(angularVelocity), std::forward<VelocityOtherT_>(velocity)) {}

  /**
   * The spatial velocity V_MBq about another point Bq fixed to B, PointOtherT_, given the position vector p_BpBq from Bp to Bq,
   * expressed in E: w_MB is unchanged, and v_MBq = v_MBp + w_MB x p_BpBq (see velocityOfBodyFixedPoint()), like Drake's
   * SpatialVelocity::Shift(). The position vector is converted into a position vector in E, so that e.g. a velocity fails with
   * "Units do not match." and a point with "Absolute/relative mismatch.".
   */
  template <EuclideanCoordinateSystem PointOtherT_, AnyTensor OffsetOtherT_>
  SpatialVelocity<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointOtherT_, ExpressedInT_> shift(const OffsetOtherT_& offset) const {
    return {this->getRotational(), velocityOfBodyFixedPoint(this->getTranslational(), this->getRotational(), offset)};
  }

  /**
   * The spatial velocity V_MCq of the frame C, which moves relative to B, measured in M, given this spatial velocity V_MBp, the
   * position vector p_BpCq from Bp to the point Cq of C, expressed in E, and the spatial velocity V_BCq of C measured in B about Cq,
   * like Drake's SpatialVelocity::ComposeWithMovingFrameVelocity(): w_MC = w_MB + w_BC, and v_MCq = v_MBp + w_MB x p_BpCq + v_BCq,
   * the velocity of the point of B at Cq plus the velocity of Cq relative to B. The frames that meet, B of this spatial velocity and
   * the frame V_BCq is measured in, must be the same ("Adjacent frames do not match."), like X_AB * X_BC. If C does not move
   * relative to B, V_BCq is zero and the result is shift().
   */
  template <AnyTensor OffsetOtherT_, typename MeasuredInOtherT_, typename FrameOtherT_, typename PointOtherT_, typename ExpressedInOtherT_>
  SpatialVelocity<ValuesT_, ValueT_, MeasuredInT_, FrameOtherT_, PointOtherT_, ExpressedInT_> composeWithMovingFrameVelocity(
      const OffsetOtherT_& offset,
      const SpatialVelocity<ValuesT_, ValueT_, MeasuredInOtherT_, FrameOtherT_, PointOtherT_, ExpressedInOtherT_>& velocity) const {
    static_assert(std::is_same_v<MeasuredInOtherT_, FrameT_>, "Adjacent frames do not match.");
    static_assert(std::is_same_v<ExpressedInOtherT_, ExpressedInT_>, "Coordinate systems do not match.");
    const auto shifted = shift<PointOtherT_>(offset);
    return {shifted.getRotational() + velocity.getRotational(), shifted.getTranslational() + velocity.getTranslational()};
  }
};

/**
 * The adjoint map [Ad_X] of the passive proper rigid transformation X_AB = (R_AB, p_AoBo_A) from B to A applied to the spatial
 * velocity V_MCbo_B of a frame C, measured in M, about the origin of B, expressed in B: the spatial velocity V_MCao_A about the
 * origin of A, expressed in A, w' = R_AB w and v' = R_AB v + p_AoBo_A x (R_AB w), i.e. re-expressed in A and shifted to the origin
 * of A. It is the adjoint representation of Modern Robotics (Lynch and Park, Cambridge University Press, 2017, section 3.3.2),
 * V_s = [Ad_T_sb] V_b, which maps the body twist V_b of the frame B to the spatial twist V_s in the frame S: here
 * adjoint(X_AB, V_ABbo_B) = V_ABao_A. Unlike X_AB.transform(), which re-expresses the spatial velocity about the same point, it
 * also moves the point, which must be the origin of B: shift() it there first otherwise. It is
 * X_AB.transform(velocity).shift<A>(-p_AoBo_A), and its 6x6 matrix is not computed.
 */
template <typename FromT_, typename ToT_, typename RotationT_, typename TranslationT_, template <typename, int, int> typename ValuesT_,
          typename ValueT_, typename MeasuredInT_, typename FrameT_, typename PointT_, typename ExpressedInT_>
auto adjoint(const PassiveProperRigidTransformationTemplate<FromT_, ToT_, RotationT_, TranslationT_>& transformation,
             const SpatialVelocity<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointT_, ExpressedInT_>& velocity) {
  return internal::adjointMap(transformation, velocity);
}

namespace internal {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename MeasuredInT_, typename FrameT_, typename PointT_,
          typename ExpressedInT_, typename NewExpressedInT_>
struct WithExpressedIn<SpatialVelocity<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointT_, ExpressedInT_>, NewExpressedInT_> {
  using type = SpatialVelocity<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointT_, NewExpressedInT_>;
};

}  // namespace internal

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename MeasuredInT_, typename FrameT_, typename PointT_,
          typename ExpressedInT_>
struct GetCoordinateSystem<SpatialVelocity<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointT_, ExpressedInT_>> {
  using type = ExpressedInT_;
};

}  // namespace wuzp
