// This example shows how wuzp prevents invalid operations on absolute quantities.
// To introduce an absolute quantity, it is required to use the Tensor class, which also specifies the corresponding coordinate system.

#include <iostream>

#include <wuzp/core/CoordinateSystem.hpp>
#include <wuzp/core/Tensor.hpp>
#include <wuzp_eigen/ValueMatrix.hpp>
#include <wuzp_units/Unit.hpp>

using namespace wuzp;        // NOLINT
using namespace wuzp_eigen;  // NOLINT
using namespace wuzp_units;  // NOLINT

struct Epoch {};
using EpochCoordinateSystem = CoordinateSystem<Epoch>;

using EpochTimePointD = TimePoint<ValueMatrix, double, EpochCoordinateSystem>;
using DurationD = Duration<ValueMatrix, double, EpochCoordinateSystem>;

int main() {
  EpochTimePointD timePoint{2.0};
  DurationD duration{3.0};
  // EpochTimePointD timePoint2 = timePoint + timePoint;  // error: static assertion failed: Two absolute objects cannot be summed.
  EpochTimePointD timePoint2 = timePoint + duration;
  std::cout << "Resulting time point: " << timePoint2 << std::endl;
  return 0;
}