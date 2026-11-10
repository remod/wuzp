// wuzp-expect: error
// wuzp-static-assert: A translation only transforms points, vectors, covectors and tensors with two upper indices: other tensors may depend on the origin, e\.g\. an inertia about it\.

// A mixed tensor may depend on the origin, e.g. the inertia about the origin written as the linear map from the angular velocity
// to the angular momentum about the origin, and its type cannot tell it from a mixed tensor that does not, e.g. a velocity
// gradient. So a translation rejects it, rather than leaving it unchanged.

#include "wuzp/trafo/PassiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Inertia = Tensor<Indices<Up, Down>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Product<Mass, Area>::type>>,
                       CoordinateSystemA, Relative>;

int main() {
  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> translation{0.0, 0.0, -1.0};
  const Inertia inertia{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = translation.transform(inertia);
  return 0;
}
