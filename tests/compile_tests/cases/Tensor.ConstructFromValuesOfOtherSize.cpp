// wuzp-expect: error
// wuzp-static-assert: Number of rows do not match\.

// A tensor can only be constructed from values of its size (#62).

#include "wuzp/core/Tensor.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

int main() {
  [[maybe_unused]] const wuzp::PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{ValueVector<double, 2>(1.0, 2.0)};
  return 0;
}
