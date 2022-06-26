#pragma once

#include <cmath>
#include <ostream>
#include <type_traits>

#include "wuzp/core/Integer.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Units.hpp"
#include "wuzp/core/ValuesWrapper.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

// Forward declaration.
template <typename ValuesT_, typename UnitsT_>
class Quantities;

/**
 * Container to store a single quantity.
 * A quantity consists of a value and a unit.
 * The value is evaluated at run-time, the unit at compile-time.
 * Defining a scalar to be equivalent to a 1x1 matrix is not 100% clean, but the implicit conversions help to avoid duplicated code and
 * explicit casting, e.g. when calculating the inner product of a vector x^T * x. https://math.stackexchange.com/q/65002
 * @tparam ValueT_ Container type to store the value.
 * @tparam UnitT_ Container storing the unit.
 */
template <typename ValueT_, typename UnitT_>
using Quantity = Quantities<ValueT_, TypeMatrixImpl<1, 1, std::tuple<UnitT_>>>;

template <typename UnitT_>
using QuantityI = Quantity<typename AccessValues<SpecializationDummy<UnitT_>>::ValueI, UnitT_>;
template <typename UnitT_>
using QuantityF = Quantity<typename AccessValues<SpecializationDummy<UnitT_>>::ValueF, UnitT_>;
template <typename UnitT_>
using QuantityD = Quantity<typename AccessValues<SpecializationDummy<UnitT_>>::ValueD, UnitT_>;

}  // namespace wuzp
