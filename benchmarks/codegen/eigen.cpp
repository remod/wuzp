// The operations of wuzp.cpp on plain Eigen, without wuzp's checks, for the comparison of the generated code (see
// ../compare_codegen.py). Each operation is a function with external linkage that takes its inputs by reference, so that the
// compiler generates it on its own and computes nothing at compile time.

#include <Eigen/Core>

namespace codegen {

// A proper rigid transformation x -> R x + t, with the members in the order of wuzp's: the rotation, then the translation.
struct RigidTransformation {
  Eigen::Matrix3d rotation;
  Eigen::Vector3d translation;
};

Eigen::Vector3d sum(const Eigen::Vector3d& lhs, const Eigen::Vector3d& rhs);
Eigen::Vector3d productWithScalar(const double& mass, const Eigen::Vector3d& acceleration);
double norm(const Eigen::Vector3d& velocity);
Eigen::Vector3d transformVector(const RigidTransformation& transformation, const Eigen::Vector3d& velocity);
Eigen::Vector3d transformPoint(const RigidTransformation& transformation, const Eigen::Vector3d& position);
RigidTransformation compose(const RigidTransformation& lhs, const RigidTransformation& rhs);

Eigen::Vector3d sum(const Eigen::Vector3d& lhs, const Eigen::Vector3d& rhs) {
  return lhs + rhs;
}

Eigen::Vector3d productWithScalar(const double& mass, const Eigen::Vector3d& acceleration) {
  return mass * acceleration;
}

double norm(const Eigen::Vector3d& velocity) {
  return velocity.norm();
}

// A vector is only rotated, a point is rotated and translated.
Eigen::Vector3d transformVector(const RigidTransformation& transformation, const Eigen::Vector3d& velocity) {
  return transformation.rotation * velocity;
}

Eigen::Vector3d transformPoint(const RigidTransformation& transformation, const Eigen::Vector3d& position) {
  return transformation.rotation * position + transformation.translation;
}

// (R_lhs, t_lhs) * (R_rhs, t_rhs) = (R_lhs R_rhs, R_lhs t_rhs + t_lhs), like wuzp's composition.
RigidTransformation compose(const RigidTransformation& lhs, const RigidTransformation& rhs) {
  return RigidTransformation{lhs.rotation * rhs.rotation, lhs.rotation * rhs.translation + lhs.translation};
}

}  // namespace codegen
