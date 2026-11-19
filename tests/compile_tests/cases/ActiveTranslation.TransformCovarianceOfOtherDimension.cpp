// wuzp-expect: error
// wuzp-static-assert: Matrix sizes do not match\.

// A translation leaves only tensors with two upper indices of its own dimension unchanged, like it only moves points of its own
// dimension: the translation of the covariance of a position in the plane by a translation in space used to return the
// covariance unchanged.

#include "wuzp/trafo/ActiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Covariance = Tensor<Indices<Up, Up>, Quantities<ValueMatrix<double, 2, 2>, TypeMatrix<2, 2, Area>>, CoordinateSystemA, Relative>;

int main() {
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> translation{0.0, 0.0, 1.0};
  const Covariance covariance{1.0, 0.5, 0.5, 2.0};
  [[maybe_unused]] const auto result = translation.transform(covariance);
  return 0;
}
