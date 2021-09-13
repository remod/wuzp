#pragma once

#include <boost/units/quantity.hpp>

namespace wuzp {

// This typedef provides the same interface as its wuzp pendant. It is used internally.
template <typename Value, typename Unit>
using Quantity = boost::units::quantity<Unit, Value>;

}  // namespace wuzp
