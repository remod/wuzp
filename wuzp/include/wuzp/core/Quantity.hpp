#pragma once

#include <cmath>
#include <ostream>
#include <tuple>
#include <type_traits>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Units.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/concepts.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

/**
 * Container to store a single quantity.
 * A quantity consists of a value and a unit.
 * The value is evaluated at run-time, the unit at compile-time.
 * Defining a scalar to be equivalent to a 1x1 matrix is not 100% clean, but the implicit conversions allow us to reuse code from the matrix
 * implementation. Also, the user can avoid explicit conversions, e.g. when calculating the inner product of a vector x^T * x.
 * https://math.stackexchange.com/q/65002
 * @tparam ValueT_ Container type to store the value.
 * @tparam UnitT_ Container storing the unit.
 */
template <typename ValueT_, typename UnitT_>
using Quantity = Quantities<ValueT_, TypeMatrixImpl<1, 1, std::tuple<UnitT_>>>;

// The aliases of quantities with a value type, e.g. QuantityD<UnitT_>, are defined by the value matrix backends, e.g.
// wuzp_eigen::QuantityD<UnitT_>, since they need a value matrix backend.

}  // namespace wuzp
