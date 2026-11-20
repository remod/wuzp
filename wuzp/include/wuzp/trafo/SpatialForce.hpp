#pragma once

#include <type_traits>
#include <utility>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformationTemplate.hpp"
#include "wuzp/trafo/SpatialVector.hpp"

namespace wuzp {

/**
 * The spatial force (wrench) F_Bp_E on the frame B, about the point Bp, expressed in the coordinate system E, in the monogram
 * notation of Drake's SpatialForce (https://drake.mit.edu/doxygen_cxx/classdrake_1_1multibody_1_1_spatial_force.html): the torque
 * tau_Bp about Bp and the force f on B, both expressed in E, the rotational part first, like Drake and Modern Robotics (Lynch and
 * Park, Cambridge University Press, 2017, section 3.4), where a wrench is (m, f). Unlike the motion vectors, it has no frame it
 * is measured in. Its tags are coordinate systems of wuzp, which must be Euclidean, like those of SpatialVelocity, and are checked
 * at compile time: spatial forces are only added and compared if all their tags match, e.g. the forces on B about Bp, and shift()
 * moves the point. A passive rotation or a passive proper rigid transformation re-expresses it about the same point, like a
 * spatial velocity, active transformations do not transform it, and adjoint() maps it by a passive proper rigid transformation to
 * the origin of To, like the wrenches of Modern Robotics. Its parts are vectors, like ForceVector and TorqueVector (see "Forces and
 * torques" in DESIGN.md), and its power with a spatial velocity V_MBp_E of B about the same point is the sum of the dot products of the
 * parts, tau . w + f . v, which neither shift() nor adjoint() changes.
 */
template <template <typename, int, int> typename ValuesT_, typename ValueT_, EuclideanCoordinateSystem FrameT_,
          EuclideanCoordinateSystem PointT_, EuclideanCoordinateSystem ExpressedInT_>
  requires AnyValueMatrix<ValuesT_>
class SpatialForce : public SpatialVector<SpatialForce<ValuesT_, ValueT_, FrameT_, PointT_, ExpressedInT_>,
                                          TorqueVector3<ValuesT_, ValueT_, ExpressedInT_>, ForceVector3<ValuesT_, ValueT_, ExpressedInT_>> {
  using BaseT =
      SpatialVector<SpatialForce, TorqueVector3<ValuesT_, ValueT_, ExpressedInT_>, ForceVector3<ValuesT_, ValueT_, ExpressedInT_>>;

 public:
  //! The frame B the force acts on.
  using FrameT = FrameT_;
  //! The point Bp the torque is about.
  using PointT = PointT_;
  //! The coordinate system E both parts are expressed in.
  using ExpressedInT = ExpressedInT_;
  //! The position vector from one point to another, expressed in E, by which shift() shifts.
  using OffsetT = PositionVector3<ValuesT_, ValueT_, ExpressedInT_>;

  SpatialForce() = default;

  //! The spatial force with the torque tau_Bp and the force f, which are converted into the torque and the force vectors in E: e.g.
  //! swapped parts fail with "Units do not match.".
  template <typename TorqueOtherT_, typename ForceOtherT_>
    requires(AnyTensor<std::remove_cvref_t<TorqueOtherT_>> && AnyTensor<std::remove_cvref_t<ForceOtherT_>>)
  SpatialForce(TorqueOtherT_&& torque, ForceOtherT_&& force)
      : BaseT(std::forward<TorqueOtherT_>(torque), std::forward<ForceOtherT_>(force)) {}

  /**
   * The spatial force F_Bq about another point Bq, PointOtherT_, given the position vector p_BpBq from Bp to Bq, expressed in E:
   * the force f is unchanged, and the torque about Bq is tau_Bq = tau_Bp - p_BpBq x f, like Drake's SpatialForce::Shift(). The
   * position vector is converted into a position vector in E, so that e.g. a force fails with "Units do not match." and a point
   * with "Absolute/relative mismatch.".
   */
  template <EuclideanCoordinateSystem PointOtherT_, AnyTensor OffsetOtherT_>
  SpatialForce<ValuesT_, ValueT_, FrameT_, PointOtherT_, ExpressedInT_> shift(const OffsetOtherT_& offset) const {
    return {this->getRotational() - OffsetT{offset}.crossProduct(this->getTranslational()), this->getTranslational()};
  }
};

/**
 * The adjoint map of the passive proper rigid transformation X_AB = (R_AB, p_AoBo_A) from B to A applied to the spatial force
 * F_Cbo_B on a frame C about the origin of B, expressed in B: the spatial force F_Cao_A about the origin of A, expressed in A,
 * f' = R_AB f and tau' = R_AB tau + p_AoBo_A x (R_AB f), i.e. re-expressed in A and shifted to the origin of A. It is the map of
 * the wrenches of Modern Robotics (Lynch and Park, Cambridge University Press, 2017, section 3.4), F_s = [Ad_T_bs]^T F_b, the
 * transpose of the adjoint map of the inverse X_BA, which preserves the power with the spatial velocities that adjoint() maps by
 * X_AB. The point the spatial force is about must be the origin of B: shift() it there first otherwise. Its 6x6 matrix is not
 * computed.
 */
template <typename FromT_, typename ToT_, typename RotationT_, typename TranslationT_, template <typename, int, int> typename ValuesT_,
          typename ValueT_, typename FrameT_, typename PointT_, typename ExpressedInT_>
auto adjoint(const PassiveProperRigidTransformationTemplate<FromT_, ToT_, RotationT_, TranslationT_>& transformation,
             const SpatialForce<ValuesT_, ValueT_, FrameT_, PointT_, ExpressedInT_>& force) {
  return internal::adjointMap(transformation, force);
}

namespace internal {

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FrameT_, typename PointT_, typename ExpressedInT_,
          typename NewExpressedInT_>
struct WithExpressedIn<SpatialForce<ValuesT_, ValueT_, FrameT_, PointT_, ExpressedInT_>, NewExpressedInT_> {
  using type = SpatialForce<ValuesT_, ValueT_, FrameT_, PointT_, NewExpressedInT_>;
};

}  // namespace internal

template <template <typename, int, int> typename ValuesT_, typename ValueT_, typename FrameT_, typename PointT_, typename ExpressedInT_>
struct GetCoordinateSystem<SpatialForce<ValuesT_, ValueT_, FrameT_, PointT_, ExpressedInT_>> {
  using type = ExpressedInT_;
};

}  // namespace wuzp
