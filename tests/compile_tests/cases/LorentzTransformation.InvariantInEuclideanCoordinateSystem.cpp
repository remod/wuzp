// wuzp-expect: error
// wuzp-static-assert: getInvariant\(\) computes the interval with the Minkowski metric, so the vector must be expressed in a Minkowski coordinate system, e\.g\. CoordinateSystem<Spacetime, Minkowski>\.

// The interval eta_ij v^i v^j uses the Minkowski metric, which a Euclidean coordinate system does not have.

#include "wuzp/trafo/LorentzTransformation.hpp"

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::getInvariant(wuzp::PositionVector<4, ValueMatrix, double, CoordinateSystemA>::zero());
  return 0;
}
