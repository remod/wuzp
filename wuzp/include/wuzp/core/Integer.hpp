#pragma once

#include "wuzp/core/Rational.hpp"

namespace wuzp {

// Rational traits also apply for integers.
template <int ValueV_>
using Integer = Rational<ValueV_, 1>;

}  // namespace wuzp
