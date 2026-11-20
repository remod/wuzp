// wuzp-expect: error
// wuzp-static-assert: Quantities are not homogeneous\.

// Only the directions of the vectors matter, but a vector whose components have different units has no direction (#77).

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "compile_test_types.hpp"

int main() {
  using Rotation = wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using MixedVector =
      wuzp::Vector<wuzp::Quantities<ValueMatrix<double, 3, 1>, wuzp::TypeMatrix<3, 1, Length, Length, Time>>, CoordinateSystemA>;
  const MixedVector from{1.0, 2.0, 3.0};
  const wuzp::PositionVector3<ValueMatrix, double, CoordinateSystemA> to{3.0, 2.0, 1.0};
  [[maybe_unused]] const auto rotation = Rotation::fromTwoVectors(from, to);
  return 0;
}
