// This example shows how wuzp prevents invalid operations on absolute quantities, which follow the rules of affine spaces.
// An absolute quantity, e.g. a time point, is a point in a one-dimensional coordinate system whose origin is its zero, here an epoch.

#include <iostream>

#include <wuzp/wuzp.hpp>
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
  // Scaling a time point depends on the epoch, so it does not compile; an affine combination is a time point plus a duration.
  // EpochTimePointD half = timePoint2 * 0.5;  // error: static assertion failed: An absolute object cannot appear in a multiplication.
  EpochTimePointD midpoint = timePoint + (timePoint2 - timePoint) * 0.5;
  std::cout << "Midpoint: " << midpoint << std::endl;
  return 0;
}