// wuzp-expect: error
// wuzp-static-assert: Quantities are not homogeneous\.

// The invariant requires all components of the vector to have the same unit.

#include "wuzp/trafo/LorentzTransformation.hpp"
#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  using MixedQuantities = Quantities<ValueMatrix<double, 4, 1>, TypeMatrix<4, 1, Length, Length, Time, Length>>;
  const Vector<MixedQuantities, CoordinateSystemA> vector{1.0, 2.0, 3.0, 4.0};
  [[maybe_unused]] const auto invariant = getInvariant(vector);
  return 0;
}
