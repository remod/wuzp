// wuzp-expect: error
// wuzp-static-assert: 'TransformationTraits' trait not specialized\.

// An active rigid transformation moves objects within its coordinate system: it would move the motion, which the frames and the
// point of the spatial velocity would no longer describe, so it does not transform spatial vectors. Passive transformations
// re-express them (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"
#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const wuzp::ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double> transformation{};
  const SpatialVelocityD<CoordinateSystemC, CoordinateSystemD, PointP, CoordinateSystemA> velocity{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                   VelocityInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto transformed = transformation.transform(velocity);
  return 0;
}
