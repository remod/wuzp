// wuzp-expect: error
// wuzp-error: no matching (member |overloaded )?function

// Euler angles are not a vector, so they are quantities without a coordinate system, not a tensor (#77).

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  using Rotation = wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  const wuzp::RotationVector3<ValueMatrix, double, CoordinateSystemA> angles{0.1, 0.2, 0.3};
  [[maybe_unused]] const auto rotation = Rotation::fromEulerAnglesZyx(angles);
  return 0;
}
