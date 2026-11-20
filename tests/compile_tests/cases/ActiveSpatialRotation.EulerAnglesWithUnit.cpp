// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// Euler angles are angles, which are dimensionless: Euler angles in metres are rejected (#77).

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  using Rotation = wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  const wuzp::Quantities<ValueMatrix<double, 3, 1>, wuzp::TypeMatrix<3, 1, Length>> angles{0.1, 0.2, 0.3};
  [[maybe_unused]] const auto rotation = Rotation::fromEulerAnglesZyx(angles);
  return 0;
}
