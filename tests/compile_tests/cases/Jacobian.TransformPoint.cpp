// wuzp-expect: error
// wuzp-static-assert: A Jacobian maps vectors, not points: the change of coordinates whose derivative it is maps the points\.

// A Jacobian is the linearization of a change of coordinates, which is not linear in general: the Jacobian of the Cartesian
// coordinates with respect to the polar coordinates at the point (2 m, pi/2) would map that point to (-3.14, 2) m instead of
// (0, 2) m. It only maps the vectors at the point, e.g. velocities.

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using JacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 2, 2, Dimensionless, Length, Dimensionless, Length>;
using PolarPoint = Point<Quantities<ValueMatrix<double, 2, 1>, TypeMatrix<2, 1, Length, Angle>>, CoordinateSystemA>;

int main() {
  const JacobianBA jacobian{0.0, -2.0, 1.0, 0.0};
  const PolarPoint point{2.0, 1.5707963267948966};
  [[maybe_unused]] const auto result = jacobian.transform(point);
  return 0;
}
