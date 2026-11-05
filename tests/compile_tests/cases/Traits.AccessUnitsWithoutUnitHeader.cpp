// wuzp-expect: error
// wuzp-static-assert: 'AccessUnits' trait not specialized for given type\. Make sure you included the necessary unit headers\.

// Using a rotation without including a units backend (e.g. wuzp_units/Unit.hpp) tells the user what is missing.

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "ValueMatrix.hpp"

struct A {};
using CoordinateSystemA = wuzp::CoordinateSystem<A>;

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>);
  return 0;
}
