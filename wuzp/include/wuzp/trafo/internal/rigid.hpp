#pragma once

#include "wuzp/core/Quantities.hpp"

namespace wuzp {
namespace internal {

// The compositions and the inverse of proper rigid transformations x -> R x + t, active or passive, computed from the
// quantities of their rotations and translations, and returned as RigidT_, whose rotation and translation are constructed from
// quantities. They follow the inverse and the composition of SE(3) in Sola, Deray and Atchuthan, "A micro Lie theory for state
// estimation in robotics" (https://arxiv.org/abs/1812.01537), eqs. 170 and 171, and Drake's RigidTransform
// (https://drake.mit.edu/doxygen_cxx/classdrake_1_1math_1_1_rigid_transform.html). The rotation of a proper rigid transformation
// is a rotation, so its inverse is its transpose, which every value matrix backend provides.

//! (R_lhs, t_lhs) * (R_rhs, t_rhs) = (R_lhs R_rhs, R_lhs t_rhs + t_lhs): x -> R_lhs (R_rhs x + t_rhs) + t_lhs.
template <typename RigidT_, typename LhsT_, typename RhsT_>
RigidT_ composeRigid(const LhsT_& lhs, const RhsT_& rhs) {
  const auto& rotation = lhs.getRotation().getQuantities();
  return RigidT_{
      typename RigidT_::Rotation{innerProduct(rotation, rhs.getRotation().getQuantities())},
      typename RigidT_::Translation{innerProduct(rotation, rhs.getTranslation().getQuantities()) + lhs.getTranslation().getQuantities()}};
}

//! (R_lhs, t_lhs) * t_rhs = (R_lhs, R_lhs t_rhs + t_lhs): x -> R_lhs (x + t_rhs) + t_lhs.
template <typename RigidT_, typename LhsT_, typename RhsT_>
RigidT_ composeRigidAndTranslation(const LhsT_& lhs, const RhsT_& rhs) {
  const auto& rotation = lhs.getRotation().getQuantities();
  return RigidT_{typename RigidT_::Rotation{rotation},
                 typename RigidT_::Translation{innerProduct(rotation, rhs.getQuantities()) + lhs.getTranslation().getQuantities()}};
}

//! t_lhs * (R_rhs, t_rhs) = (R_rhs, t_rhs + t_lhs): x -> R_rhs x + t_rhs + t_lhs.
template <typename RigidT_, typename LhsT_, typename RhsT_>
RigidT_ composeTranslationAndRigid(const LhsT_& lhs, const RhsT_& rhs) {
  return RigidT_{typename RigidT_::Rotation{rhs.getRotation().getQuantities()},
                 typename RigidT_::Translation{rhs.getTranslation().getQuantities() + lhs.getQuantities()}};
}

//! (R, t)^-1 = (R^T, -R^T t): x -> R^T (x - t).
template <typename RigidT_, typename T_>
RigidT_ invertRigid(const T_& rigid) {
  const auto transposedRotation = rigid.getRotation().getQuantities().transpose();
  return RigidT_{typename RigidT_::Rotation{transposedRotation},
                 typename RigidT_::Translation{-innerProduct(transposedRotation, rigid.getTranslation().getQuantities())}};
}

}  // namespace internal
}  // namespace wuzp
