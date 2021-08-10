#pragma once

#include <boost/units/quantity.hpp>

namespace ctm {
namespace units {

// Note: For this convenience typedef we swap the arguments to be consistent with CTM.
template <typename Value, typename Unit>
using Quantity = boost::units::quantity<Unit, Value>;

}  // namespace units
}  // namespace ctm
