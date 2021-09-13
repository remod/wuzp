#pragma once

#include "ctm/core/Rational.hpp"

namespace ctm {

// Rational traits also apply for integers.
template <int Value>
using Integer = Rational<Value, 1>;

}  // namespace ctm
