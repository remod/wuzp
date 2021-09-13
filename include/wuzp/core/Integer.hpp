#pragma once

#include "wuzp/core/Rational.hpp"

namespace wuzp {

// Rational traits also apply for integers.
template <int Value>
using Integer = Rational<Value, 1>;

}  // namespace wuzp
