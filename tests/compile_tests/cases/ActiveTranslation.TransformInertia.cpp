// wuzp-expect: error
// wuzp-static-assert: A translation only transforms points, vectors, covectors and tensors with two upper indices: other tensors may depend on the origin, e\.g\. an inertia about it\.

// A tensor with two lower indices may be an inertia about the origin, which changes with the origin by the parallel axis
// theorem, and its type cannot tell it from a tensor that does not, e.g. a metric. So a translation rejects it, rather than
// leaving it unchanged.

#include "wuzp/trafo/ActiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Inertia = Tensor<Indices<Down, Down>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Product<Mass, Area>::type>>,
                       CoordinateSystemA, Relative>;

int main() {
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> translation{0.0, 0.0, 1.0};
  const Inertia inertia{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = translation.transform(inertia);
  return 0;
}
