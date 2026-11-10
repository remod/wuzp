// wuzp-expect: error
// wuzp-static-assert: A translation only transforms points, vectors, covectors and tensors with two upper indices: other tensors may depend on the origin, e\.g\. an inertia about it\.

// A scalar may depend on the origin, e.g. the potential energy m g z of a mass at the height z, and its type cannot tell it from
// a scalar that does not, e.g. a mass. So a translation rejects it, rather than leaving it unchanged.

#include "wuzp/trafo/ActiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using PotentialEnergy = Tensor<Indices<>, Quantities<ValueMatrix<double, 1, 1>, TypeMatrix<1, 1, Energy>>, CoordinateSystemA, Relative>;

int main() {
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> translation{0.0, 0.0, 1.0};
  const PotentialEnergy potentialEnergy{9.81};
  [[maybe_unused]] const auto result = translation.transform(potentialEnergy);
  return 0;
}
