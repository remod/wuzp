#pragma once

#include <type_traits>
#include <utility>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/helpers.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformationTemplate.hpp"
#include "wuzp/trafo/SpatialVector.hpp"
#include "wuzp/trafo/SpatialVelocity.hpp"
#include "wuzp/trafo/kinematics.hpp"

namespace wuzp {

/**
 * The spatial acceleration A_MBp_E of the frame B, measured in the frame M, about the point Bp, expressed in the coordinate system
 * E, in the monogram notation of Drake's SpatialAcceleration
 * (https://drake.mit.edu/doxygen_cxx/classdrake_1_1multibody_1_1_spatial_acceleration.html): the angular acceleration alpha_MB of
 * B relative to M and the acceleration a_MBp of the point Bp, which moves with B, relative to M, both the derivatives in M of the
 * parts of the spatial velocity V_MBp, expressed in E, the rotational part first. The tags are those of SpatialVelocity, and are
 * checked like them. Shifting it to another point needs the angular velocity w_MB of B, for the centripetal acceleration, which
 * shift() and composeWithMovingFrameAcceleration() take as a spatial velocity of B measured in M, so that its tags are checked
 * too. A passive rotation or a passive proper rigid transformation re-expresses it, like a spatial velocity, about the same point,
 * and active transformations do not transform it. It has no adjoint map, since its shift needs the angular
 * velocity: re-express it and shift() it instead (see adjoint()). These are the accelerations of Drake, the derivatives of the
 * velocities of points, not the spatial accelerations of Featherstone, the derivatives of the spatial velocity about a point fixed
 * in M (see "Spatial vectors" in DESIGN.md).
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_, EuclideanCoordinateSystem MeasuredInT_,
          EuclideanCoordinateSystem FrameT_, EuclideanCoordinateSystem PointT_, EuclideanCoordinateSystem ExpressedInT_>
  requires AnyValueMatrix<ValuesT_>
class SpatialAcceleration : public SpatialVector<SpatialAcceleration<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointT_, ExpressedInT_>,
                                                 AngularAccelerationVector3<ValuesT_, ValueT_, ExpressedInT_>,
                                                 AccelerationVector3<ValuesT_, ValueT_, ExpressedInT_>> {
  using BaseT = SpatialVector<SpatialAcceleration, AngularAccelerationVector3<ValuesT_, ValueT_, ExpressedInT_>,
                              AccelerationVector3<ValuesT_, ValueT_, ExpressedInT_>>;

 public:
  //! The frame M the motion is measured in.
  using MeasuredInT = MeasuredInT_;
  //! The frame B whose motion it is.
  using FrameT = FrameT_;
  //! The point Bp the spatial acceleration is about, which is fixed to B.
  using PointT = PointT_;
  //! The coordinate system E both parts are expressed in.
  using ExpressedInT = ExpressedInT_;
  //! The position vector from one point to another, expressed in E, by which shift() shifts.
  using OffsetT = PositionVector3<ValuesT_, ValueT_, ExpressedInT_>;

  SpatialAcceleration() = default;

  //! The spatial acceleration with the angular acceleration alpha_MB and the acceleration a_MBp, which are converted into the
  //! angular acceleration and the acceleration vectors in E: e.g. swapped parts fail with "Units do not match.".
  template <typename AngularAccelerationOtherT_, typename AccelerationOtherT_>
    requires(AnyTensor<std::remove_cvref_t<AngularAccelerationOtherT_>> && AnyTensor<std::remove_cvref_t<AccelerationOtherT_>>)
  SpatialAcceleration(AngularAccelerationOtherT_&& angularAcceleration, AccelerationOtherT_&& acceleration)
      : BaseT(std::forward<AngularAccelerationOtherT_>(angularAcceleration), std::forward<AccelerationOtherT_>(acceleration)) {}

  /**
   * The spatial acceleration A_MBq about another point Bq fixed to B, PointOtherT_, given the position vector p_BpBq from Bp to Bq,
   * expressed in E, and the spatial velocity V_MB of B measured in M, about any point, expressed in E, whose angular velocity w_MB
   * it takes: alpha_MB is unchanged, and a_MBq = a_MBp + alpha_MB x p_BpBq + w_MB x (w_MB x p_BpBq) (see
   * accelerationOfBodyFixedPoint()), like Drake's SpatialAcceleration::Shift(). The spatial velocity must be measured in M ("Measured-in
   * frames do not match."), be that of B ("Frames do not match.") and be expressed in E ("Coordinate systems do not match.").
   */
  template <EuclideanCoordinateSystem PointOtherT_, AnyTensor OffsetOtherT_, typename MeasuredInVelocityT_, typename FrameVelocityT_,
            typename PointVelocityT_, typename ExpressedInVelocityT_>
  SpatialAcceleration<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointOtherT_, ExpressedInT_> shift(
      const OffsetOtherT_& offset,
      const SpatialVelocity<ValuesT_, ValueT_, MeasuredInVelocityT_, FrameVelocityT_, PointVelocityT_, ExpressedInVelocityT_>& velocity)
      const {
    static_assert(internal::SameSpatialTagsExceptPoint<SpatialAcceleration, std::remove_cvref_t<decltype(velocity)>>::value);
    return {this->getRotational(),
            accelerationOfBodyFixedPoint(this->getTranslational(), this->getRotational(), velocity.getRotational(), offset)};
  }

  /**
   * The spatial acceleration A_MCq of the frame C, which moves relative to B, measured in M, given this spatial acceleration A_MBp,
   * the position vector p_BpCq from Bp to the point Cq of C, expressed in E, the spatial velocity V_MB of B measured in M, about any
   * point, and the spatial velocity V_BCq and the spatial acceleration A_BCq of C measured in B about Cq, like Drake's
   * SpatialAcceleration::ComposeWithMovingFrameAcceleration():
   * alpha_MC = alpha_MB + alpha_BC + w_MB x w_BC and
   * a_MCq = a_MBp + alpha_MB x p_BpCq + w_MB x (w_MB x p_BpCq) + 2 w_MB x v_BCq + a_BCq,
   * the acceleration of the point of B at Cq, the Coriolis acceleration and the acceleration of Cq relative to B. (The note of
   * Drake's documentation starts the sum with a_BCo instead of a_MBo, a typo: its derivation, and the tests, have a_MBo.) V_MB is
   * checked like for shift(), V_BCq and A_BCq must have the same tags ("Reference points do not match.", etc.), and the frame they
   * are measured in must be B ("Adjacent frames do not match."). If C does not move relative to B, the result is shift().
   */
  template <AnyTensor OffsetOtherT_, typename MeasuredInVelocityT_, typename FrameVelocityT_, typename PointVelocityT_,
            typename ExpressedInVelocityT_, typename MeasuredInRelativeT_, typename FrameRelativeT_, typename PointRelativeT_,
            typename ExpressedInRelativeT_, typename MeasuredInAccelerationT_, typename FrameAccelerationT_, typename PointAccelerationT_,
            typename ExpressedInAccelerationT_>
  SpatialAcceleration<ValuesT_, ValueT_, MeasuredInT_, FrameRelativeT_, PointRelativeT_, ExpressedInT_> composeWithMovingFrameAcceleration(
      const OffsetOtherT_& offset,
      const SpatialVelocity<ValuesT_, ValueT_, MeasuredInVelocityT_, FrameVelocityT_, PointVelocityT_, ExpressedInVelocityT_>&
          velocityOfThisFrame,
      const SpatialVelocity<ValuesT_, ValueT_, MeasuredInRelativeT_, FrameRelativeT_, PointRelativeT_, ExpressedInRelativeT_>& velocity,
      const SpatialAcceleration<ValuesT_, ValueT_, MeasuredInAccelerationT_, FrameAccelerationT_, PointAccelerationT_,
                                ExpressedInAccelerationT_>& acceleration) const {
    static_assert(internal::SameSpatialTags<std::remove_cvref_t<decltype(velocity)>, std::remove_cvref_t<decltype(acceleration)>>::value);
    static_assert(std::is_same_v<MeasuredInRelativeT_, FrameT_>, "Adjacent frames do not match.");
    static_assert(std::is_same_v<ExpressedInRelativeT_, ExpressedInT_>, "Coordinate systems do not match.");
    const auto shifted = shift<PointRelativeT_>(offset, velocityOfThisFrame);
    // Not named angularVelocity: MSVC checks the names of locals against the globals at the point of instantiation, and warns if
    // they hide one, e.g. a global angularVelocity of the user (C4459, #84).
    const auto& angularVelocityOfThisFrame = velocityOfThisFrame.getRotational();
    return {shifted.getRotational() + acceleration.getRotational() + angularVelocityOfThisFrame.crossProduct(velocity.getRotational()),
            shifted.getTranslational() + angularVelocityOfThisFrame.crossProduct(velocity.getTranslational()) * ValueT_{2} +
                acceleration.getTranslational()};
  }
};

/**
 * Rejected with a message: the adjoint map of a rigid transformation maps spatial velocities and spatial forces, but a spatial
 * acceleration of Drake shifts with the angular velocity of its frame (see SpatialAcceleration::shift()). Re-express it with the
 * rigid transformation, X_AB.transform(acceleration), and shift() it. The return type is deduced, so that the message is the first
 * error (see FailOnInstantiation).
 */
template <typename FromT_, typename ToT_, typename RotationT_, typename TranslationT_, template <typename, int, int> typename ValuesT_,
          typename ValueT_, typename MeasuredInT_, typename FrameT_, typename PointT_, typename ExpressedInT_>
auto adjoint(const PassiveProperRigidTransformationTemplate<FromT_, ToT_, RotationT_, TranslationT_>& /*transformation*/,
             const SpatialAcceleration<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointT_, ExpressedInT_>& /*acceleration*/) {
  static_assert(FailOnInstantiation<SpatialAcceleration<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointT_, ExpressedInT_>>::value,
                "The adjoint map of a rigid transformation maps spatial velocities and spatial forces. Shifting a spatial acceleration "
                "needs the angular velocity: re-express it with transform() and shift() it.");
}

namespace internal {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename MeasuredInT_, typename FrameT_, typename PointT_,
          typename ExpressedInT_, typename NewExpressedInT_>
struct WithExpressedIn<SpatialAcceleration<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointT_, ExpressedInT_>, NewExpressedInT_> {
  using type = SpatialAcceleration<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointT_, NewExpressedInT_>;
};

}  // namespace internal

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename MeasuredInT_, typename FrameT_, typename PointT_,
          typename ExpressedInT_>
struct GetCoordinateSystem<SpatialAcceleration<ValuesT_, ValueT_, MeasuredInT_, FrameT_, PointT_, ExpressedInT_>> {
  using type = ExpressedInT_;
};

}  // namespace wuzp
