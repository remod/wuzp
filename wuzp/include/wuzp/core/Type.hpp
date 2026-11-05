#pragma once

#include "wuzp/core/TypeMatrix.hpp"

namespace wuzp {

template <typename TypeT_>
using Type = TypeMatrix<1, 1, TypeT_>;

}  // namespace wuzp
