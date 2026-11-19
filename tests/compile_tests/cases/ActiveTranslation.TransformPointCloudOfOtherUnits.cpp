// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// A point cloud is translated by adding the values of the translation to those of every point, after checking the units of this
// sum at compile time, like for a single point: the units of every component must be those of the translation.

#include "wuzp/trafo/ActiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

// Three points in space and time, whose second component is a time.
using PointCloud =
    Point<Quantities<ValueMatrix<double, 2, 3>, TypeMatrix<2, 3, Length, Length, Length, Time, Time, Time>>, CoordinateSystemA>;

int main() {
  const ActiveSpatialTranslation2<ValueMatrix, double, CoordinateSystemA> translation{0.0, 1.0};
  const PointCloud points{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  [[maybe_unused]] const auto result = translation.transform(points);
  return 0;
}
