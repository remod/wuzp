#pragma once

#include <utility>

#include "wuzp/core/Quantities.hpp"

namespace wuzp {
namespace internal {

// The compositions and the inverse of proper rigid transformations x -> R x + t, active or passive, with each other, with
// rotations and with translations, computed from the quantities of their rotations and translations, and returned as RigidT_,
// whose rotation and translation are constructed from quantities. They follow the inverse and the composition of SE(3) in Sola,
// Deray and Atchuthan, "A micro Lie theory for state estimation in robotics" (https://arxiv.org/abs/1812.01537), eqs. 170 and
// 171, and Drake's RigidTransform (https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rigid_transform.html), where a
// rotation R is the rigid transformation (R, 0) and a translation t is (I, t). The rotation of a proper rigid transformation is
// a rotation, so its inverse is its transpose, which every value matrix backend provides.

//! The proper rigid transformation RigidT_ with the given rotation and translation. The rotation is a product or a transpose of
//! rotations, so it is a rotation and is not checked again, like the products of Drake's RotationMatrix (see makeUnchecked()).
template <typename RigidT_, typename RotationQuantitiesT_, typename TranslationQuantitiesT_>
RigidT_ makeRigid(RotationQuantitiesT_&& rotation, TranslationQuantitiesT_&& translation) {
  return RigidT_{RigidT_::Rotation::makeUnchecked(std::forward<RotationQuantitiesT_>(rotation)),
                 typename RigidT_::Translation{std::forward<TranslationQuantitiesT_>(translation)}};
}

//! (R_lhs, t_lhs) * (R_rhs, t_rhs) = (R_lhs R_rhs, R_lhs t_rhs + t_lhs): x -> R_lhs (R_rhs x + t_rhs) + t_lhs.
template <typename RigidT_, typename LhsT_, typename RhsT_>
RigidT_ composeRigid(const LhsT_& lhs, const RhsT_& rhs) {
  const auto& rotation = lhs.getRotation().getQuantities();
  return makeRigid<RigidT_>(innerProduct(rotation, rhs.getRotation().getQuantities()),
                            innerProduct(rotation, rhs.getTranslation().getQuantities()) + lhs.getTranslation().getQuantities());
}

//! (R_lhs, t_lhs) * t_rhs = (R_lhs, R_lhs t_rhs + t_lhs): x -> R_lhs (x + t_rhs) + t_lhs.
template <typename RigidT_, typename LhsT_, typename RhsT_>
RigidT_ composeRigidAndTranslation(const LhsT_& lhs, const RhsT_& rhs) {
  const auto& rotation = lhs.getRotation().getQuantities();
  return makeRigid<RigidT_>(rotation, innerProduct(rotation, rhs.getQuantities()) + lhs.getTranslation().getQuantities());
}

//! t_lhs * (R_rhs, t_rhs) = (R_rhs, t_rhs + t_lhs): x -> R_rhs x + t_rhs + t_lhs.
template <typename RigidT_, typename LhsT_, typename RhsT_>
RigidT_ composeTranslationAndRigid(const LhsT_& lhs, const RhsT_& rhs) {
  return makeRigid<RigidT_>(rhs.getRotation().getQuantities(), rhs.getTranslation().getQuantities() + lhs.getQuantities());
}

//! (R_lhs, t_lhs) * R_rhs = (R_lhs R_rhs, t_lhs): x -> R_lhs R_rhs x + t_lhs.
template <typename RigidT_, typename LhsT_, typename RhsT_>
RigidT_ composeRigidAndRotation(const LhsT_& lhs, const RhsT_& rhs) {
  return makeRigid<RigidT_>(innerProduct(lhs.getRotation().getQuantities(), rhs.getQuantities()), lhs.getTranslation().getQuantities());
}

//! R_lhs * (R_rhs, t_rhs) = (R_lhs R_rhs, R_lhs t_rhs): x -> R_lhs (R_rhs x + t_rhs).
template <typename RigidT_, typename LhsT_, typename RhsT_>
RigidT_ composeRotationAndRigid(const LhsT_& lhs, const RhsT_& rhs) {
  const auto& rotation = lhs.getQuantities();
  return makeRigid<RigidT_>(innerProduct(rotation, rhs.getRotation().getQuantities()),
                            innerProduct(rotation, rhs.getTranslation().getQuantities()));
}

//! R_lhs * t_rhs = (R_lhs, R_lhs t_rhs): x -> R_lhs (x + t_rhs).
template <typename RigidT_, typename LhsT_, typename RhsT_>
RigidT_ composeRotationAndTranslation(const LhsT_& lhs, const RhsT_& rhs) {
  return makeRigid<RigidT_>(lhs.getQuantities(), innerProduct(lhs.getQuantities(), rhs.getQuantities()));
}

//! t_lhs * R_rhs = (R_rhs, t_lhs): x -> R_rhs x + t_lhs.
template <typename RigidT_, typename LhsT_, typename RhsT_>
RigidT_ composeTranslationAndRotation(const LhsT_& lhs, const RhsT_& rhs) {
  return makeRigid<RigidT_>(rhs.getQuantities(), lhs.getQuantities());
}

//! (R, t)^-1 = (R^T, -R^T t): x -> R^T (x - t).
template <typename RigidT_, typename T_>
RigidT_ invertRigid(const T_& rigid) {
  auto transposedRotation = rigid.getRotation().getQuantities().transpose();
  auto translation = -innerProduct(transposedRotation, rigid.getTranslation().getQuantities());
  return makeRigid<RigidT_>(std::move(transposedRotation), std::move(translation));
}

}  // namespace internal
}  // namespace wuzp
