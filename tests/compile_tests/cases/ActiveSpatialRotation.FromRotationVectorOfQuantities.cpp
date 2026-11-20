// wuzp-expect: error
// wuzp-error: no matching (member |overloaded )?function

// A rotation vector is a vector in the coordinate system of the rotation, a tensor, not quantities without a coordinate system,
// which the rotation would take as expressed in its own (#77).

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  using Rotation = wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  const wuzp::Quantities<ValueMatrix<double, 3, 1>, wuzp::TypeMatrix<3, 1, Angle>> rotationVector{0.1, 0.2, 0.3};
  [[maybe_unused]] const auto rotation = Rotation::fromRotationVector(rotationVector);
  return 0;
}
