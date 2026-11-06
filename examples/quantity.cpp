// This example shows how wuzp prevents mixing quantities with incompatible units.

#include <iostream>

#include <wuzp/wuzp.hpp>
#include <wuzp_eigen/ValueMatrix.hpp>
#include <wuzp_units/Unit.hpp>

using namespace wuzp;        // NOLINT
using namespace wuzp_eigen;  // NOLINT
using namespace wuzp_units;  // NOLINT

using MassQuantityD = QuantityD<Mass>;
using ForceQuantityD = QuantityD<Force>;
using AccelerationQuantityD = QuantityD<Acceleration>;

int main() {
  MassQuantityD mass{2.0};
  AccelerationQuantityD acceleration{3.0};
  // ForceQuantityD force = mass;  // error: static assertion failed: Units do not match.
  ForceQuantityD force = mass * acceleration;
  std::cout << "Resulting force: " << force << std::endl;
  return 0;
}