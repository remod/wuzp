// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// The passive rotation from two vectors takes the first one in From and the second one in To, whose direction the first one
// has in To (#77).

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  using Rotation = wuzp::PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB>;
  const wuzp::PositionVector3<ValueMatrix, double, CoordinateSystemA> from{1.0, 2.0, 3.0};
  const wuzp::PositionVector3<ValueMatrix, double, CoordinateSystemA> to{3.0, 2.0, 1.0};
  [[maybe_unused]] const auto rotation = Rotation::fromTwoVectors(from, to);
  return 0;
}
