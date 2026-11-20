// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The parts of a spatial vector are tensors, so that their operations check the units and the coordinate systems: quantities,
// which have no coordinate system, are not a rotational part (#76).

#include "wuzp/trafo/SpatialVector.hpp"

#include "spatial_vector_types.hpp"

struct MotionVector;

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::SpatialVector<MotionVector, AngularVelocityInA::QuantitiesT, VelocityInA>);
  return 0;
}
