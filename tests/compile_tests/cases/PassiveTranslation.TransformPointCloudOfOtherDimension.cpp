// wuzp-expect: error
// wuzp-static-assert: Matrix sizes do not match\.

// A translation only translates points of its own dimension, also in a point cloud: the translation of a cloud of points in the
// plane by a translation in space used to add its first two components to every point and to drop the third (#58).

#include "wuzp/trafo/PassiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using PointCloud = Point<Quantities<ValueMatrix<double, 2, 3>, TypeMatrix<2, 3, Length>>, CoordinateSystemA>;

int main() {
  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> translation{0.0, 0.0, -1.0};
  const PointCloud points{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  [[maybe_unused]] const auto result = translation.transform(points);
  return 0;
}
