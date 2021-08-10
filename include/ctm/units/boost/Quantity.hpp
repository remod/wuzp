#pragma once

#include <boost/units/quantity.hpp>

namespace ctm {
namespace units {

// This typedef provides the same interface as its ctm pendant. It is used internally.
template <typename Value, typename Unit>
using Quantity = boost::units::quantity<Unit, Value>;

}  // namespace units
}  // namespace ctm
