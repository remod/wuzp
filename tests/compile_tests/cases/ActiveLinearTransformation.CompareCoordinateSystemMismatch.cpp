// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Only linear transformations in the same coordinate system can be compared.

#include "wuzp/trafo/ActiveLinearTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

template <typename CoordinateSystemT>
using Scaling = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemT, 3, 3, Dimensionless>;

int main() {
  const Scaling<CoordinateSystemA> scalingInA{2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0};
  const Scaling<CoordinateSystemB> scalingInB{2.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 2.0};
  return scalingInA == scalingInB ? 0 : 1;
}
